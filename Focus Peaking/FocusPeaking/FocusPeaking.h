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

/*
	FocusPeaking.h
*/

#pragma once

#ifndef FOCUS_PEAKING_H
#define FOCUS_PEAKING_H

typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned short		u_int16;
typedef unsigned long		u_long;
typedef short int			int16;
#define PF_TABLE_BITS	12
#define PF_TABLE_SZ_16	4096

#define PF_DEEP_COLOR_AWARE 1	// make sure we get 16bpc pixels; 
								// AE_Effect.h checks for this.

#include "AEConfig.h"

#ifdef AE_OS_WIN
	typedef unsigned short PixelType;
	#include <Windows.h>
#endif

#include "entry.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"
#include "AE_EffectCBSuites.h"
#include "String_Utils.h"
#include "AE_GeneralPlug.h"
#include "AEFX_ChannelDepthTpl.h"
#include "AEGP_SuiteHandler.h"

#include "FocusPeaking_Strings.h"

/* Versioning information */

#define	MAJOR_VERSION	1
#define	MINOR_VERSION	0
#define	BUG_VERSION		0
#define	STAGE_VERSION	PF_Stage_DEVELOP
#define	BUILD_VERSION	1


/* Parameter defaults */

#define	FOCUS_PEAKING_THRESHOLD_MIN		0
#define	FOCUS_PEAKING_THRESHOLD_MAX		100000
#define	FOCUS_PEAKING_THRESHOLD_DFLT		10

enum {
	FOCUS_PEAKING_INPUT = 0,
	FOCUS_PEAKING_THRESHOLD,
	FOCUS_PEAKING_COLOR,
	FOCUS_PEAKING_ONLY,
	FOCUS_PEAKING_ALPHA,
	FOCUS_PEAKING_NUM_PARAMS
};

enum {
	THRESHOLD_DISK_ID = 1,
	COLOR_DISK_ID = 2,
	ONLY_DISK_ID = 3,
	ALPHA_DISK_ID = 4
};

typedef struct ThresholdInfo{
	PF_FpLong	thresholdF;
	PF_FpLong prevInt8;
	PF_FpLong prevInt16;
	PF_Pixel	color;
	PF_Boolean  isOnly;
	PF_Boolean  Alpha;
} ThresholdInfo, *ThresholdInfoP, **ThresholdInfoH;

#ifdef __cplusplus
	extern "C" {
#endif
	
DllExport	PF_Err 
EntryPointFunc(	
	PF_Cmd			cmd,
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output,
	void			*extra) ;

#ifdef __cplusplus
}
#endif

#endif // FOCUS_PEAKING_H