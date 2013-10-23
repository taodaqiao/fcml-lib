/*
 * fcml_rend_intel.c
 *
 *  Created on: Oct 20, 2013
 *      Author: tas
 */

#include "fcml_utils.h"
#include "fcml_rend_intel.h"
#include "fcml_rend_utils.h"

#define FCML_REND_LOCAL_BUFFER_SIZE 512

void fcml_ifn_rend_print_prefixes_intel( fcml_st_asm_dec_prefixes *prefixes ) {
	// TODO: Prefixes, when they are added.
}

fcml_ceh_error fcml_ifn_rend_operand_renderer_immediate_intel(  fcml_st_dialect_context *dialect_context, fcml_st_memory_stream *output_stream, fcml_st_disassembler_result *result, fcml_st_operand *operand, fcml_st_asm_dec_operand_details *operand_details, fcml_uint32_t render_flags ) {
	fcml_st_immediate *immediate = &(operand->immediate);
	fcml_st_integer integer;
	fcml_ceh_error error = fcml_fn_utils_imm_to_integer( immediate, &integer );
	if( error ) {
		return error;
	}
	return fcml_fn_rend_utils_format_append_integer( output_stream, &integer, render_flags & FCML_REND_FLAG_HEX_IMM );
}

fcml_ceh_error fcml_ifn_rend_operand_renderer_reg_intel( fcml_st_dialect_context *dialect_context, fcml_st_memory_stream *output_stream, fcml_st_disassembler_result *result, fcml_st_operand *operand, fcml_st_asm_dec_operand_details *operand_details, fcml_uint32_t render_flags ) {
	fcml_fn_rend_utils_format_append_reg( dialect_context, output_stream, &(operand->reg), result->prefixes.is_rex );
	return FCML_CEH_GEC_NO_ERROR;
}

fcml_ceh_error fcml_ifn_rend_operand_renderer_address_intel( fcml_st_dialect_context *dialect_context, fcml_st_memory_stream *output_stream, fcml_st_disassembler_result *result, fcml_st_operand *operand, fcml_st_asm_dec_operand_details *operand_details, fcml_uint32_t render_flags ) {

	fcml_ceh_error error = FCML_CEH_GEC_NO_ERROR;

	fcml_st_address *address = &(operand->address);

	if( dialect_context->size_operator_renderer && address->effective_address.size_operator > 0 ) {
		fcml_char buffer[32] = {0};
		dialect_context->size_operator_renderer( address->effective_address.size_operator, buffer, sizeof( buffer ), operand_details->hints & FCML_OP_HINT_MULTIMEDIA_INSTRUCTION );
		fcml_fn_rend_utils_format_append_str( output_stream, buffer );
	}

	if( !address->segment_selector.is_default_reg || ( render_flags & FCML_REND_FLAG_RENDER_DEFAULT_SEG ) ) {
		fcml_fn_rend_utils_format_append_reg( dialect_context, output_stream, &(address->segment_selector.segment_selector), result->prefixes.is_rex );
		fcml_fn_rend_utils_format_append_str( output_stream, ":" );
	}

	fcml_fn_rend_utils_format_append_str( output_stream, "[" );

	fcml_st_effective_address *effective_address = &(address->effective_address);

	fcml_bool first = FCML_TRUE;

	if( address->address_form == FCML_AF_COMBINED ) {

		// Append base register.
		if( !fcml_fn_utils_is_reg_undef( &(effective_address->base) ) ) {
			fcml_fn_rend_utils_format_append_reg( dialect_context, output_stream, &(effective_address->base), result->prefixes.is_rex );
			first = FCML_FALSE;
		}

		// Append index register.
		if( !fcml_fn_utils_is_reg_undef( &(effective_address->index) ) ) {
			fcml_fn_rend_utils_format_append_str_if( output_stream, "+", !first );
			fcml_fn_rend_utils_format_append_reg( dialect_context, output_stream, &(effective_address->index), result->prefixes.is_rex );
			first = FCML_FALSE;
		}

		// Append scale.
		if( effective_address->scale_factor > 0 ) {

			fcml_fn_rend_utils_format_append_str_if( output_stream, "*", !first );
			first = FCML_FALSE;

			fcml_st_integer scale_value = {0};
			scale_value.is_signed = FCML_FALSE;
			scale_value.size = 8;
			scale_value.int8 = (fcml_int8_t)effective_address->scale_factor;

			fcml_fn_rend_utils_format_append_integer( output_stream, &scale_value, FCML_FALSE );
		}

		// Displacement.
		if( effective_address->displacement.size > 0 ) {

			fcml_fn_rend_utils_format_append_str_if( output_stream, "+", !first );
			first = FCML_FALSE;

			fcml_st_integer integer;
			error = fcml_fn_utils_displacement_to_integer( &(effective_address->displacement), &integer );
			if( error ) {
				return error;
			}

			error = fcml_fn_rend_utils_format_append_integer( output_stream, &integer, render_flags & FCML_REND_FLAG_HEX_DISPLACEMENT );
		}

	} else {

		// Offset.

		fcml_st_integer integer;
		fcml_ceh_error error = fcml_fn_utils_offset_to_integer( &(address->offset), &integer );
		if( error ) {
			return error;
		}

		error = fcml_fn_rend_utils_format_append_integer( output_stream, &integer, FCML_TRUE );
	}

	fcml_fn_rend_utils_format_append_str( output_stream, "]" );

	return error;
}

fcml_ceh_error fcml_ifn_rend_operand_renderer_far_pointer_intel( fcml_st_dialect_context *dialect_context, fcml_st_memory_stream *output_stream, fcml_st_disassembler_result *result, fcml_st_operand *operand, fcml_st_asm_dec_operand_details *operand_details, fcml_uint32_t render_flags ) {

	fcml_ceh_error error = FCML_CEH_GEC_NO_ERROR;

	fcml_st_integer integer = {0};
	integer.is_signed = FCML_TRUE;
	integer.size = 16;
	integer.int16 = operand->far_pointer.segment;

	fcml_fn_rend_utils_format_append_integer( output_stream, &integer, FCML_TRUE );

	fcml_fn_rend_utils_format_append_str( output_stream, ":" );

	integer.size = operand->far_pointer.offset_size;
	switch( integer.size ) {
	case FCML_DS_16:
		integer.int16 = operand->far_pointer.offset16;
		break;
	case FCML_DS_32:
		integer.int32 = operand->far_pointer.offset32;
		break;
	}

	fcml_fn_rend_utils_format_append_integer( output_stream, &integer, FCML_TRUE );

	return error;
}

fcml_ceh_error fcml_ifn_rend_print_operand_intel(  fcml_st_dialect_context *dialect_context, fcml_st_memory_stream *output_stream, fcml_st_disassembler_result *result, fcml_int operand_index, fcml_uint32_t render_flags ) {
	fcml_ceh_error error = FCML_CEH_GEC_NO_ERROR;
	fcml_st_operand *operand = &(result->operands[operand_index]);
	switch( operand->type ) {
	case FCML_EOT_IMMEDIATE:
		error = fcml_ifn_rend_operand_renderer_immediate_intel( dialect_context, output_stream, result, operand, &(result->operand_details[operand_index]), render_flags );
		break;
	case FCML_EOT_FAR_POINTER:
		error = fcml_ifn_rend_operand_renderer_far_pointer_intel( dialect_context, output_stream, result, operand, &(result->operand_details[operand_index]), render_flags );
		break;
	case FCML_EOT_ADDRESS:
		error = fcml_ifn_rend_operand_renderer_address_intel( dialect_context, output_stream, result, operand, &(result->operand_details[operand_index]), render_flags );
		break;
	case FCML_EOT_REGISTER:
		error = fcml_ifn_rend_operand_renderer_reg_intel( dialect_context, output_stream, result, operand, &(result->operand_details[operand_index]), render_flags );
		break;
	case FCML_EOT_NONE:
		break;
	default:
		error = FCML_CEH_GEC_INVALID_INPUT;
	}
	return error;
}

fcml_ceh_error fcml_fn_rend_render_instruction_intel( fcml_st_dialect_context *dialect_context, fcml_st_memory_stream *output_stream, fcml_st_disassembler_result *result, fcml_uint32_t render_flags ) {

	fcml_char local_buffer[FCML_REND_LOCAL_BUFFER_SIZE] = {0};

	fcml_ceh_error error = FCML_CEH_GEC_NO_ERROR;

	// Local stream.
	fcml_st_memory_stream local_stream;
	local_stream.base_address = local_buffer;
	local_stream.size = sizeof( local_buffer );
	local_stream.offset = 0;

	// Instruction code.
	if( render_flags & FCML_REND_FLAG_RENDER_CODE ) {
		fcml_fn_rend_utils_format_append_code( output_stream, result->instruction_code, result->instruction_size );
		fcml_fn_rend_utils_format_append_str( output_stream, " " );
	}

	fcml_ifn_rend_print_prefixes_intel( &result->prefixes );

	// Mnemonic.
	fcml_fn_rend_utils_format_append_str( output_stream, result->mnemonic );

	// Short form, so operands should be ignored.
	if( result->is_shortcut ) {
		return error;
	}

	// Add hints.
	if( result->hints & FCML_HINT_FAR_POINTER ) {
		fcml_fn_rend_utils_format_append_str( output_stream, " " );
		fcml_fn_rend_utils_format_append_str( output_stream, "far" );
	}

	// Add all operands.
	fcml_int i;
	for( i = 0; i < FCML_OPERANDS_COUNT; i++ ) {
		if( result->operands[i].type != FCML_EOT_NONE ) {
			error = fcml_ifn_rend_print_operand_intel( dialect_context, &local_stream, result, i, render_flags );
			if( !error ) {
				fcml_fn_rend_utils_format_append_str( output_stream, ( i > 0 )  ? "," : " " );
				fcml_fn_rend_utils_format_append_stream( output_stream, &local_stream );
				fcml_fn_stream_clean( &local_stream );
			} else {
				break;
			}
		} else {
			break;
		}
	}

	return error;
}