/*
 * fcml_utils_t.c
 *
 *  Created on: 25-06-2013
 *      Author: tAs
 */


#include "fcml_utils_t.h"

#include <fcml_utils.h>

int fcml_tf_utils_suite_init(void) {
	return 0;
}

int fcml_tf_utils_suite_cleanup(void) {
	return 0;
}

void fcml_fn_utils_test_sign_convert_imm_to_int8(void) {

	fcml_int8_t value;

	fcml_st_integer imm;

	imm.size = FCML_DS_8;
	imm.int8 = 0xFF;
	imm.is_signed = FCML_FALSE;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int8( &imm, &value ), FCML_CEH_GEC_NO_ERROR );
	CU_ASSERT_EQUAL( value, (fcml_int8_t)-1 );

	imm.size = FCML_DS_8;
	imm.int8 = 45;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int8( &imm, &value ), FCML_CEH_GEC_NO_ERROR );
	CU_ASSERT_EQUAL( value, (fcml_int8_t)45 );

	imm.size = FCML_DS_16;
	imm.int16 = 0xFFFF;
	imm.is_signed = FCML_FALSE;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int8( &imm, &value ), FCML_CEH_GEC_VALUE_OUT_OF_RANGE );

	imm.size = FCML_DS_16;
	imm.int16 = 0xFFFF;
	imm.is_signed = FCML_TRUE;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int8( &imm, &value ), FCML_CEH_GEC_NO_ERROR );
	CU_ASSERT_EQUAL( value, (fcml_int8_t)-1 );

	imm.size = FCML_DS_32;
	imm.int32 = 0xFFFFFFFF;
	imm.is_signed = FCML_FALSE;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int8( &imm, &value ), FCML_CEH_GEC_VALUE_OUT_OF_RANGE );

	imm.size = FCML_DS_32;
	imm.int32 = 0xFFFFFFFF;
	imm.is_signed = FCML_TRUE;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int8( &imm, &value ), FCML_CEH_GEC_NO_ERROR );
	CU_ASSERT_EQUAL( value, (fcml_int8_t)-1 );

	imm.size = FCML_DS_32;
	imm.int32 = 0x00FFFFFF;
	imm.is_signed = FCML_FALSE;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int8( &imm, &value ), FCML_CEH_GEC_VALUE_OUT_OF_RANGE );

	imm.size = FCML_DS_16;
	imm.int16 = 129;
	imm.is_signed = FCML_TRUE;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int8( &imm, &value ), FCML_CEH_GEC_VALUE_OUT_OF_RANGE );

	imm.size = FCML_DS_32;
	imm.int32 = 129;
	imm.is_signed = FCML_FALSE;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int8( &imm, &value ), FCML_CEH_GEC_VALUE_OUT_OF_RANGE );

}

void fcml_fn_utils_test_sign_convert_imm_to_int16(void) {

	fcml_int16_t value;

	fcml_st_integer imm;

	imm.size = FCML_DS_8;
	imm.int8 = 0xFF;
	imm.is_signed = FCML_TRUE;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int16( &imm, &value ), FCML_CEH_GEC_NO_ERROR );
	CU_ASSERT_EQUAL( value, (fcml_int16_t)-1 );

	imm.size = FCML_DS_8;
	imm.int8 = 45;
	imm.is_signed = FCML_TRUE;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int16( &imm, &value ), FCML_CEH_GEC_NO_ERROR );
	CU_ASSERT_EQUAL( value, (fcml_int16_t)45 );

	imm.size = FCML_DS_16;
	imm.int16 = 0xFFFF;
	imm.is_signed = FCML_TRUE;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int16( &imm, &value ), FCML_CEH_GEC_NO_ERROR );
	CU_ASSERT_EQUAL( value, (fcml_int16_t)-1 );

	imm.size = FCML_DS_16;
	imm.int16 = 0xFFFF;
	imm.is_signed = FCML_FALSE;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int16( &imm, &value ), FCML_CEH_GEC_NO_ERROR );
	CU_ASSERT_EQUAL( value, (fcml_int16_t)-1 );

	imm.size = FCML_DS_32;
	imm.int32 = 0xFFFFFFFF;
	imm.is_signed = FCML_TRUE;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int16( &imm, &value ), FCML_CEH_GEC_NO_ERROR );
	CU_ASSERT_EQUAL( value, (fcml_int16_t)-1 );

	imm.size = FCML_DS_32;
	imm.int32 = 0x00FFFFFF;
	imm.is_signed = FCML_TRUE;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int16( &imm, &value ), FCML_CEH_GEC_VALUE_OUT_OF_RANGE );

	imm.size = FCML_DS_32;
	imm.int32 = 32768;
	imm.is_signed = FCML_TRUE;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int16( &imm, &value ), FCML_CEH_GEC_VALUE_OUT_OF_RANGE );

}

void fcml_fn_utils_test_sign_convert_imm_to_int32(void) {

	fcml_int32_t value;

	fcml_st_integer imm;

	imm.size = FCML_DS_8;
	imm.int8 = 0xFF;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int32( &imm, &value ), FCML_CEH_GEC_NO_ERROR );
	CU_ASSERT_EQUAL( value, (fcml_int32_t)-1 );

	imm.size = FCML_DS_8;
	imm.int8 = 45;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int32( &imm, &value ), FCML_CEH_GEC_NO_ERROR );
	CU_ASSERT_EQUAL( value, (fcml_int32_t)45 );

	imm.size = FCML_DS_16;
	imm.int16 = 0xFFFF;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int32( &imm, &value ), FCML_CEH_GEC_NO_ERROR );
	CU_ASSERT_EQUAL( value, (fcml_int32_t)-1 );

	imm.size = FCML_DS_32;
	imm.int32 = 0xFFFFFFFF;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int32( &imm, &value ), FCML_CEH_GEC_NO_ERROR );
	CU_ASSERT_EQUAL( value, (fcml_int32_t)-1 );

	imm.size = FCML_DS_32;
	imm.int32 = 0x00FFFFFF;
	CU_ASSERT_EQUAL( fcml_fn_utils_convert_integer_to_int32( &imm, &value ), FCML_CEH_GEC_NO_ERROR );
	CU_ASSERT_EQUAL( value, (fcml_int32_t)0x00FFFFFF );

}

CU_TestInfo fcml_ti_utils[] = {
    { "fcml_fn_utils_test_sign_convert_imm_to_int8", fcml_fn_utils_test_sign_convert_imm_to_int8 },
    { "fcml_fn_utils_test_sign_convert_imm_to_int16", fcml_fn_utils_test_sign_convert_imm_to_int16 },
    { "fcml_fn_utils_test_sign_convert_imm_to_int32", fcml_fn_utils_test_sign_convert_imm_to_int32 },
    CU_TEST_INFO_NULL,
};

CU_SuiteInfo fcml_si_utils[] = {
    { "suite-fcml-utils", fcml_tf_utils_suite_init, fcml_tf_utils_suite_cleanup, fcml_ti_utils },
    CU_SUITE_INFO_NULL,
};



