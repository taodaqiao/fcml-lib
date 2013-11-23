/*
 * fcml_x64intel_asm_parser.c
 *
 *  Created on: 20-03-2013
 *      Author: tAs
 */

#include <string.h>
#include "fcml_ceh.h"
#include "fcml_env.h"
#include "fcml_common.h"
#include "fcml_errors.h"
#include "fcml_parser_data.h"
#include "fcml_x64att_parser.tab.h"
#include "fcml_x64att_lexer.h"
#include "fcml_x64att_asm_parser.h"
#include "fcml_utils.h"
#include "fcml_messages.h"

void att_error( struct fcml_st_parser_data *pd, const char *error ) {
	// Stores parser error into standard container.
	fcml_fn_ceh_add_error( &(pd->errors), (const fcml_string)error, FCML_EN_X64IP_ERROR_INVALID_SYNTAX, FCML_EN_CEH_EL_ERROR );
}

fcml_ceh_error fcml_ifn_att_parser_far_pointer_correction( fcml_st_ceh_error_container *errors, fcml_st_instruction *instruction ) {

	fcml_ceh_error error = FCML_CEH_GEC_NO_ERROR;

	// Correction for ambiguous FAR_POINTER addressing mode. Unfortunately we can not treat it as two IMM operands.
	if( instruction->operands_count == 2 && instruction->operands[0].type == FCML_EOT_IMMEDIATE && instruction->operands[1].type == FCML_EOT_IMMEDIATE ) {

		fcml_st_integer segment = {0};
		fcml_st_integer offset = {0};

		error = fcml_fn_utils_imm_to_integer( &(instruction->operands[0].immediate), &segment );
		if( error ) {
			// Should never happened, because parser is responsible for setting appropriate IMM size.
			return error;
		}

		error = fcml_fn_utils_imm_to_integer( &(instruction->operands[1].immediate), &offset );
		if( error ) {
			// Should never happened, because parser is responsible for setting appropriate IMM size.
			return error;
		}

		// Clean operands.
		fcml_fn_env_memory_clear( instruction->operands, sizeof( instruction->operands ) );

		fcml_st_far_pointer *far_pointer = &(instruction->operands[0].far_pointer);
		instruction->operands[0].type = FCML_EOT_FAR_POINTER;
		instruction->operands_count = 1;

		error = fcml_fn_utils_convert_integer_to_uint16( &segment, &(far_pointer->segment) );
		if( !error ) {
			error = fcml_fn_utils_convert_integer_to_uint16( &offset, &(far_pointer->offset16) );
			if( error ) {
				error = fcml_fn_utils_convert_integer_to_uint32( &offset, &(far_pointer->offset32) );
				if( !error ) {
					far_pointer->offset_size = FCML_DS_32;
				}
			} else {
				far_pointer->offset_size = FCML_DS_16;
			}
		}
		if( error ) {
			fcml_fn_ceh_add_error( errors, fcml_fn_msg_get_message( FCML_MC_GEN_VALUE_OUT_OF_RANGE ), FCML_EN_APC_AST_ERROR_VALUE_OUT_OF_RANGE, FCML_EN_CEH_EL_ERROR );
		}
	}

	return error;
}

fcml_ceh_error fcml_x64_att_parse( fcml_st_dialect *dialect, fcml_string asm_code, fcml_st_parser_result **result ) {

	fcml_ceh_error error = FCML_CEH_GEC_NO_ERROR;

	*result = NULL;

	fcml_st_parser_data parser = {0};

	/* Set up scanner. */
	if ( att_lex_init_extra( &parser, &(parser.scannerInfo) ) ) {
		return FCML_CEH_GEC_OUT_OF_MEMORY;
	}

	/*Instruction size is limited to prevent from parser's stack and buffer overflow.*/
	if( strlen( asm_code ) > FCML_PARSER_MAX_INSTRUCTION_LEN ) {
		att_lex_destroy( parser.scannerInfo );
		return FCML_CEH_GEC_INVALID_INPUT;
	}

	att__scan_string( asm_code, parser.scannerInfo );

	fcml_st_parser_result *tmp_result = (fcml_st_parser_result*)fcml_fn_env_memory_alloc_clear( sizeof( fcml_st_parser_result ) );
	if( tmp_result == NULL ) {
		return FCML_CEH_GEC_OUT_OF_MEMORY;
	}

	memset( tmp_result, 0, sizeof( fcml_st_parser_result ) );

	int yyresult = att_parse(&parser);

	att_lex_destroy( parser.scannerInfo );

	// Copy errors from parser.
	tmp_result->errors = parser.errors;

	if( yyresult ) {
		*result = tmp_result;
		switch( yyresult ) {
		case 1: /*Syntax error.*/
			error = FCML_CEH_GEC_INVALID_INPUT;
			break;
		case 2: /*Out of memory*/
			error = FCML_CEH_GEC_OUT_OF_MEMORY;
			break;
		}
		return error;
	}

	error = fcml_fn_ast_to_cif_converter( parser.tree, &(tmp_result->errors), &(tmp_result->instruction) );
	if( error ) {
		// Free instruction, because it might haven't been fully parsed.
		fcml_fn_ast_free_converted_cif( tmp_result->instruction );
		tmp_result->instruction = NULL;
	} else {
		error = fcml_ifn_att_parser_far_pointer_correction( &(tmp_result->errors), tmp_result->instruction );
	}

	fcml_fn_ast_free_node( parser.tree );

	*result = tmp_result;

	return error;
}