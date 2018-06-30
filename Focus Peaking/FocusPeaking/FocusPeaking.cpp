/*******************************************************************/
/*                                                                 */
/*                      ADOBE CONFIDENTIAL                         */
/*                   _ _ _ _ _ _ _ _ _ _ _ _ _                     */
/*                                                                 */
/* Copyright 2007 Adobe Systems Incorporated                       */
/* All Rights Reserved.                                            */
/*                                                                 */
/* NOTICE:  All information contained herein is, and remains the   */
/* property of Adobe Systems Incorporated and its suppliers, if    */
/* any.  The intellectual and technical concepts contained         */
/* herein are proprietary to Adobe Systems Incorporated and its    */
/* suppliers and may be covered by U.S. and Foreign Patents,       */
/* patents in process, and are protected by trade secret or        */
/* copyright law.  Dissemination of this information or            */
/* reproduction of this material is strictly forbidden unless      */
/* prior written permission is obtained from Adobe Systems         */
/* Incorporated.                                                   */
/*                                                                 */
/*******************************************************************/

#include "FocusPeaking.h"

static PF_Err 
About (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	AEGP_SuiteHandler suites(in_data->pica_basicP);
	
	suites.ANSICallbacksSuite1()->sprintf(	out_data->return_msg,
											"%s v%d.%d\r%s",
											STR(StrID_Name), 
											MAJOR_VERSION, 
											MINOR_VERSION, 
											STR(StrID_Description));
	return PF_Err_NONE;
}

static PF_Err 
GlobalSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	out_data->my_version = PF_VERSION(	MAJOR_VERSION, 
										MINOR_VERSION,
										BUG_VERSION, 
										STAGE_VERSION, 
										BUILD_VERSION);

	out_data->out_flags =  PF_OutFlag_DEEP_COLOR_AWARE;	// just 16bpc, not 32bpc
	
	return PF_Err_NONE;
}

static PF_Err ParamsSetup (PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef	*output )
{
	PF_Err		err		= PF_Err_NONE;
	PF_ParamDef	def;	

	AEFX_CLR_STRUCT(def);

	PF_ADD_FLOAT_SLIDERX(	STR(StrID_Threshold_Param_Name), 
							FOCUS_PEAKING_THRESHOLD_MIN, 
							FOCUS_PEAKING_THRESHOLD_MAX, 
							FOCUS_PEAKING_THRESHOLD_MIN, 
							FOCUS_PEAKING_THRESHOLD_MAX, 
							FOCUS_PEAKING_THRESHOLD_DFLT,
							PF_Precision_INTEGER,
							0,
							0,
							THRESHOLD_DISK_ID);

	AEFX_CLR_STRUCT(def);

	PF_ADD_COLOR(	STR(StrID_Color_Param_Name), 
					PF_MAX_CHAN8,
					PF_MAX_CHAN8,
					0,
					COLOR_DISK_ID);

	AEFX_CLR_STRUCT(def);

	PF_ADD_CHECKBOX(STR(StrID_Only_Param_Name), 0, 0, 0, ONLY_DISK_ID);

	AEFX_CLR_STRUCT(def);

	PF_ADD_CHECKBOX(STR(StrID_Alpha_Param_Name), 0, 0, 0, ONLY_DISK_ID);

	AEFX_CLR_STRUCT(def);
	
	out_data->num_params = FOCUS_PEAKING_NUM_PARAMS;

	return err;
}

static PF_Err FocusPeaking16 (void *refcon, A_long xL, A_long yL, PF_Pixel16 *inP, PF_Pixel16 *outP)
{
	PF_Err		err = PF_Err_NONE;

	ThresholdInfo	*tiP	= reinterpret_cast<ThresholdInfo*>(refcon);
					
	if (tiP){
		outP->alpha		=	inP->alpha; // process the alpha no matter what
		PF_FpLong average = (inP->red + inP->green + inP->blue) / (PF_FpLong)3; // take the average of the three channels
		PF_FpLong threshold = tiP->thresholdF / 100;
		PF_Pixel color = tiP->color;
		if (ABS(average - tiP->prevInt16) > threshold && xL != 0) {
			// the output is changed to the specific color
			outP->red		=	color.red * PF_MAX_CHAN16 / PF_MAX_CHAN8;
			outP->green		=	color.green * PF_MAX_CHAN16 / PF_MAX_CHAN8;
			outP->blue		=	color.blue * PF_MAX_CHAN16 / PF_MAX_CHAN8;
		} else {
			if (!tiP->isOnly) {
				// the output remains the same
				outP->red		=	inP->red;
				outP->green		=	inP->green;
				outP->blue		=	inP->blue;
			} else {
				outP->red       =   0;
				outP->green       =   0;
				outP->blue       =   0;
			}

			if (tiP->Alpha) outP->alpha = 0;
		}

		tiP->prevInt16 = average;
	}

	return err;
}

static PF_Err FocusPeaking8 (void *refcon, A_long xL, A_long yL, PF_Pixel8 *inP, PF_Pixel8 *outP)
{
	PF_Err		err = PF_Err_NONE;

	ThresholdInfo	*tiP	= reinterpret_cast<ThresholdInfo*>(refcon);
					
	if (tiP){
		outP->alpha		=	inP->alpha; // process the alpha no matter what
		PF_FpLong average = (inP->red + inP->green + inP->blue) / (PF_FpLong)3; // take the average of the three channels
		PF_FpLong threshold = tiP->thresholdF / 100;
		PF_Pixel color = tiP->color;
		if (ABS(average - tiP->prevInt8) > threshold && xL != 0) {
			// the output is changed to the specific color
			outP->red		=	(A_u_char)color.red;
			outP->green		=	(A_u_char)color.green;
			outP->blue		=	(A_u_char)color.blue;
		} else {
			if (!tiP->isOnly) {
				// the output remains the same
				outP->red		=	inP->red;
				outP->green		=	inP->green;
				outP->blue		=	inP->blue;
			} else {
				outP->red       =   0;
				outP->green     =   0;
				outP->blue      =   0;
			}

			if (tiP->Alpha) outP->alpha = 0;
		}

		tiP->prevInt8 = average;
	}

	return err;
}

static PF_Err 
Render (PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output)
{
	PF_Err				err		= PF_Err_NONE;
	AEGP_SuiteHandler	suites(in_data->pica_basicP);

	/*	Put interesting code here. */
	ThresholdInfo			tiP;
	AEFX_CLR_STRUCT(tiP);
	A_long				linesL	= 0;

	linesL 		    = output->extent_hint.bottom - output->extent_hint.top;
	tiP.thresholdF 	= params[FOCUS_PEAKING_THRESHOLD]->u.fs_d.value;
	tiP.color       = params[FOCUS_PEAKING_COLOR]->u.cd.value;
	tiP.isOnly      = params[FOCUS_PEAKING_ONLY]->u.bd.value;
	tiP.Alpha       = params[FOCUS_PEAKING_ALPHA]->u.bd.value;
	
	if (PF_WORLD_IS_DEEP(output)){
		ERR(suites.Iterate16Suite1()->iterate(	in_data,
												0,								// progress base
												linesL,							// progress final
												&params[FOCUS_PEAKING_INPUT]->u.ld,	// src 
												NULL,							// area - null for all pixels
												(void*)&tiP,					// refcon - your custom data pointer
												FocusPeaking16,				    // pixel function pointer
												output));
	} else {
		ERR(suites.Iterate8Suite1()->iterate(	in_data,
												0,								// progress base
												linesL,							// progress final
												&params[FOCUS_PEAKING_INPUT]->u.ld,	// src 
												NULL,							// area - null for all pixels
												(void*)&tiP,					// refcon - your custom data pointer
												FocusPeaking8,				    // pixel function pointer
												output));	
	}

	return err;
}


DllExport	
PF_Err 
EntryPointFunc (
	PF_Cmd			cmd,
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output,
	void			*extra)
{
	PF_Err		err = PF_Err_NONE;
	
	try {
		switch (cmd) {
			case PF_Cmd_ABOUT:

				err = About(in_data,
							out_data,
							params,
							output);
				break;
				
			case PF_Cmd_GLOBAL_SETUP:

				err = GlobalSetup(	in_data,
									out_data,
									params,
									output);
				break;
				
			case PF_Cmd_PARAMS_SETUP:

				err = ParamsSetup(	in_data,
									out_data,
									params,
									output);
				break;
				
			case PF_Cmd_RENDER:

				err = Render(	in_data,
								out_data,
								params,
								output);
				break;
		}
	}
	catch(PF_Err &thrown_err){
		err = thrown_err;
	}
	return err;
}

