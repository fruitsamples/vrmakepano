//////////////	File:		QTVRUtilities.c////	Contains:	Some utilities for working with QuickTime VR movies.//				All utilities start with the prefix "QTVRUtils_".////	Written by:	Tim Monroe////	Copyright:	� 1996-1998 by Apple Computer, Inc., all rights reserved.////	Change History (most recent first):////	   <26>	 	09/27/00	rtm		modified QTVRUtils_AddStr255ToAtomContainer so it doesn't add a null byte to//									the string in the atom container; also added some bullet-proofing//	   <25>	 	04/15/99	rtm		added QTVRUtils_PanAngleToPoint3D, from Bryce Wolfson//	   <24>	 	04/05/99	rtm		added QTVRUtils_IsBackBufferHorizontal//	   <23>	 	03/11/99	rtm		moved _GetControllerType and _SetControllerType to QTUtilities//	   <22>	 	02/03/99	rtm		moved non-QTVR-specific utilities to QTUtilities//	   <21>	 	02/12/98	rtm		added QTVRUtils_HideHotSpotNames and her sisters, *Show* and *Toggle*//	   <20>	 	01/27/98	rtm		revised QTVRUtils_IsQTVRMgrInstalled and QTVRUtils_GetQTVRVersion//	   <19>	 	01/26/98	rtm		revised QTVRUtils_GetHotSpotName to look also in hot spot atom for name atom//	   <18>	 	01/14/98	rtm		added QTVRUtils_SetControllerType and QTVRUtils_AddStr255ToAtomContainer//	   <17>	 	10/20/97	rtm		added QTVRUtils_IsMultiNode; added Endian*_BtoN macros to file format routines//	   <16>	 	10/17/97	rtm		fixed QTVRUtils_IsControllerButtonVisible behavior for speaker button//	   <15>	 	10/07/97	rtm		added cannotFindAtomErr result code to QTVRUtils_Get*AtomData functions//	   <14>	 	09/15/97	rtm		added QTVRUtils_ToggleControllerBar//	   <13>	 	08/21/97	rtm		added QTVRUtils_IsControllerButtonVisible//	   <12>	 	08/19/97	rtm		added #ifdefs to support Windows compilation//	   <11>	 	08/05/97	rtm		added QTVRUtils_GetNodeComment; still needs testing//	   <10>	 	07/27/97	rtm		fixed QTVRUtils_GetHotSpotCount; added QTVRUtils_GetHotSpotIDByIndex//	   <9>	 	07/25/97	rtm		revised QTVRUtils_Get*AtomData functions to use QTCopyAtomDataToPtr;//									rewrote QTVRUtils_GetStringFromAtom//	   <8>	 	07/24/97	rtm		removed sound volume utilities; added QTVRUtils_IsZoomAvailable;//									revised QTVRUtils_IsQTVRMovie to use GetUserDataItem, not GetUserData//	   <7>	 	07/23/97	rtm		revised file format utilities; added QTVRUtils_Get*AtomData functions//	   <6>	 	07/22/97	rtm		fixed QTVRUtils_GetHotSpotCount to make sure handle is actually resized//	   <5>	 	07/21/97	rtm		added QTVRUtils_GetNodeCount//	   <4>	 	06/04/97	rtm		fixed QTVRUtils_ShowControllerButton and QTVRUtils_HideControllerButton,//									and added some explanation of them; added QTVRUtils_ResetControllerButton//	   <3>	 	02/03/97	rtm		revised QTVRUtils_ShowControllerButton and QTVRUtils_HideControllerButton //									to use explicit flag//	   <2>	 	12/03/96	rtm		added controller bar utilities//	   <1>	 	11/27/96	rtm		first file//	   //////////////////////// header files////////////#ifndef __QTVRUtilities__#include "QTVRUtilities.h"#endif#ifndef __QTUtilities__#include "QTUtilities.h"#endif#include <math.h>#include <stdlib.h>#include <string.h>/////////////////////////////////////////////////////////////////////////////////////////////////////////////// General utilities.//// Use these functions to get information about the availability/features of QuickTime VR or other services./////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// QTVRUtils_IsQTVRMgrInstalled// Is the QuickTime VR Manager installed?////////////Boolean QTVRUtils_IsQTVRMgrInstalled (void) {	Boolean 	myQTVRAvail = false;	long		myAttrs;	OSErr 		myErr = noErr;	myErr = Gestalt(gestaltQTVRMgrAttr, &myAttrs);	if (myErr == noErr)		if (myAttrs & (1L << gestaltQTVRMgrPresent))			myQTVRAvail = true;	return(myQTVRAvail);}////////////// QTVRUtils_GetQTVRVersion// Get the version of the QuickTime VR Manager installed.//// The low-order word of the returned long integer contains the version number,// so you can test a version like this:////		if (QTVRUtils_GetQTVRVersion() < 0x0210)		// we require QTVR 2.1 or greater//			return;////////////long QTVRUtils_GetQTVRVersion (void){	long	 	myVersion = 0L;	OSErr 		myErr = noErr;	myErr = Gestalt(gestaltQTVRMgrVers, &myVersion);	if (myErr == noErr)		return(myVersion);	else		return(0L);}////////////// QTVRUtils_IsQTVRMovie// Is the specified movie a QTVR movie?//// WARNING: This function is intended for use ONLY when you want to determine if you've got a QTVR movie// but you don't want to use the QuickTime VR API (perhaps QTVR isn't installed...). The preferred way to// determine if a movie is a QTVR movie is to call QTVRGetQTVRTrack and then QTVRGetQTVRInstance; if you// get back a non-NULL instance, you've got a QTVR movie.////////////Boolean QTVRUtils_IsQTVRMovie (Movie theMovie) {	Boolean			myIsQTVRMovie = false;	OSType			myType;		// QTVR movies have a special piece of user data identifying the movie controller type	myType = QTUtils_GetControllerType(theMovie);		if ((myType == kQTVRQTVRType) || (myType == kQTVROldPanoType) || (myType == kQTVROldObjectType))		myIsQTVRMovie = true; 			return(myIsQTVRMovie);}////////////// QTVRUtils_Is20QTVRMovie// Is the specified QTVR movie version 2.0 or greater?////////////Boolean QTVRUtils_Is20QTVRMovie (Movie theMovie) {	Boolean			myIs20QTVRMovie = false;	OSType			myType;		// QTVR movies have a special piece of user data identifying the movie controller type	myType = QTUtils_GetControllerType(theMovie);		if (myType == kQTVRQTVRType)		myIs20QTVRMovie = true; 			return(myIs20QTVRMovie);}////////////// QTVRUtils_IsTranslateAvailable// Is translation currently enabled for the specified object node?////////////Boolean QTVRUtils_IsTranslateAvailable (QTVRInstance theInstance) {	Boolean		myState;		QTVRGetControlSetting(theInstance, kQTVRTranslation, &myState);	return(myState);}////////////// QTVRUtils_IsZoomAvailable// Is zooming currently enabled for the specified object node?////////////Boolean QTVRUtils_IsZoomAvailable (QTVRInstance theInstance) {	Boolean		myState;		QTVRGetControlSetting(theInstance, kQTVRCanZoom, &myState);	return(myState);}////////////// QTVRUtils_IsPanoNode// Is the specified node a panoramic node?////////////Boolean QTVRUtils_IsPanoNode (QTVRInstance theInstance) {	return(QTVRGetNodeType(theInstance, kQTVRCurrentNode) == kQTVRPanoramaType);}////////////// QTVRUtils_IsObjectNode// Is the specified node an object node?////////////Boolean QTVRUtils_IsObjectNode (QTVRInstance theInstance) {	return(QTVRGetNodeType(theInstance, kQTVRCurrentNode) == kQTVRObjectType);}////////////// QTVRUtils_IsHotSpotInNode// Does the specified node contain at least one hot spot (whether visible, enabled, or whatever)?//// NOTE: This is not an easy function to implement using just the QTVR 2.1 API. We do have our own// utility QTVRUtils_GetHotSpotCount, but that function returns the number of hot spot information atoms// in the node, which is not (necessarily) the number of hot spot regions in the hot spot image track.// For panoramas, we could check to see if the panorama sample atom structure contains a reference// to a hot spot image track; if it does, we'd blindly assume that that track isn't empty. For objects,// we'll have to rely on QTVRUtils_GetHotSpotCount. So it goes....//// In an ideal world, there would be a hot spot information atom for each and every hot spot region in// the hot spot image track, in which case we could be happier using QTVRUtils_GetHotSpotCount.////////////Boolean QTVRUtils_IsHotSpotInNode (QTVRInstance theInstance) {	return(QTVRUtils_GetHotSpotCount(theInstance, QTVRGetCurrentNodeID(theInstance), NULL) > 0);}////////////// QTVRUtils_IsMultiNode// Does the specified QuickTime VR instance contain more than one node?////////////Boolean QTVRUtils_IsMultiNode (QTVRInstance theInstance) {	return(QTVRUtils_GetNodeCount(theInstance) > (UInt32)1);}////////////// QTVRUtils_IsBackBufferHorizontal// Is the back buffer oriented horizontally?////////////Boolean QTVRUtils_IsBackBufferHorizontal (QTVRInstance theInstance){	UInt32		myGeometry;	UInt16		myResolution;	UInt32		myCachePixelFormat;	SInt16		myCacheSize;	OSErr		myErr = noErr;	if (theInstance == NULL)		return(false);	myErr = QTVRGetBackBufferSettings(theInstance, &myGeometry, &myResolution, &myCachePixelFormat, &myCacheSize);	if (myErr != noErr)		return(false);	else		return(myGeometry == kQTVRHorizontalCylinder);}////////////// QTVRUtils_HideHotSpotNames// Disable the displaying of hot spot names in the controller bar.////////////void QTVRUtils_HideHotSpotNames (MovieController theMC) {	QTUtils_HideControllerButton(theMC, kQTUtilsHotSpotNames);}////////////// QTVRUtils_ShowHotSpotNames// Enable the displaying of hot spot names in the controller bar.////////////void QTVRUtils_ShowHotSpotNames (MovieController theMC) {	QTUtils_ShowControllerButton(theMC, kQTUtilsHotSpotNames);}////////////// QTVRUtils_ToggleHotSpotNames// Toggle the displaying of hot spot names in the controller bar.////////////void QTVRUtils_ToggleHotSpotNames (MovieController theMC) {	QTUtils_ToggleControllerButton(theMC, kQTUtilsHotSpotNames);}/////////////////////////////////////////////////////////////////////////////////////////////////////////////// File format utilities.//// Use these functions to read information from QuickTime VR files that's not accessible using the API.// Throughout, we assume that we're dealing with format 2.0 files. We begin with a series of functions that// return a pointer to the data in an atom (QTVRUtils_Get*AtomData); you probably won't use these functions// directly.//// Keep in mind that data stored in QuickTime atoms is big-endian. We'll need to convert any multi-byte data// that we read from an atom to native format before we use it.//// Note that these file format utilities are all Getters. As yet, no Setters. Perhaps later?/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// QTVRUtils_GetVRWorldHeaderAtomData// Get a pointer to the VR world header atom data in a QTVR movie.////////////OSErr QTVRUtils_GetVRWorldHeaderAtomData (QTVRInstance theInstance, QTVRWorldHeaderAtomPtr theVRWorldHdrAtomPtr){	QTAtomContainer			myVRWorld;	QTAtom					myAtom;	OSErr					myErr = noErr;			// get the VR world	myErr = QTVRGetVRWorld(theInstance, &myVRWorld);	if (myErr != noErr)		return(myErr);		// get the single VR world header atom in the VR world	myAtom = QTFindChildByIndex(myVRWorld, kParentAtomIsContainer, kQTVRWorldHeaderAtomType, 1, NULL);	if (myAtom != 0)		myErr = QTCopyAtomDataToPtr(myVRWorld, myAtom, false, sizeof(QTVRWorldHeaderAtom), theVRWorldHdrAtomPtr, NULL);	else 		myErr = cannotFindAtomErr;		QTDisposeAtomContainer(myVRWorld);	return(myErr);}////////////// QTVRUtils_GetNodeHeaderAtomData// Get a pointer to the node header atom data for the node having the specified node ID.////////////OSErr QTVRUtils_GetNodeHeaderAtomData (QTVRInstance theInstance, UInt32 theNodeID, QTVRNodeHeaderAtomPtr theNodeHdrPtr){	QTAtomContainer			myNodeInfo;	QTAtom					myAtom;	OSErr					myErr = noErr;			// get the node information atom container for the specified node	myErr = QTVRGetNodeInfo(theInstance, theNodeID, &myNodeInfo);	if (myErr != noErr)		return(myErr);		// get the single node header atom in the node information atom container	myAtom = QTFindChildByID(myNodeInfo, kParentAtomIsContainer, kQTVRNodeHeaderAtomType, 1, NULL);	if (myAtom != 0)		myErr = QTCopyAtomDataToPtr(myNodeInfo, myAtom, false, sizeof(QTVRNodeHeaderAtom), theNodeHdrPtr, NULL);	else 		myErr = cannotFindAtomErr;	QTDisposeAtomContainer(myNodeInfo);	return(myErr);}////////////// QTVRUtils_GetHotSpotAtomData// Get a pointer to the hot spot atom data for hot spot having the specified hot spot ID in the specified node.////////////OSErr QTVRUtils_GetHotSpotAtomData (QTVRInstance theInstance, UInt32 theNodeID, UInt32 theHotSpotID, QTVRHotSpotInfoAtomPtr theHotSpotInfoPtr){	QTAtomContainer			myNodeInfo;	QTAtom					myHSParentAtom;	OSErr					myErr = noErr;			// (1) the node information atom container contains a *hot spot parent atom*;	// (2) the hot spot parent atom contains one or more *hot spot atoms*;	// (3) the hot spot atom contains two children, a *general hot spot information atom*	//     and a *specific hot spot information atom*.	// We want to return a pointer to the general hot spot information atom data.	// get the node information atom container for the specified node	myErr = QTVRGetNodeInfo(theInstance, theNodeID, &myNodeInfo);	if (myErr != noErr)		return(myErr);		// get the single hot spot parent atom in the node information atom container	myHSParentAtom = QTFindChildByID(myNodeInfo, kParentAtomIsContainer, kQTVRHotSpotParentAtomType, 1, NULL);	if (myHSParentAtom != 0) {		QTAtom				myHSAtom;				// get the hot spot atom whose atom ID is the specified hot spot ID		myHSAtom = QTFindChildByID(myNodeInfo, myHSParentAtom, kQTVRHotSpotAtomType, theHotSpotID, NULL);		if (myHSAtom != 0) {			QTAtom			myAtom;						// get the single hot spot information atom in the hot spot atom			myAtom = QTFindChildByIndex(myNodeInfo, myHSAtom, kQTVRHotSpotInfoAtomType, 1, NULL);			if (myAtom != 0) {				myErr = QTCopyAtomDataToPtr(myNodeInfo, myAtom, false, sizeof(QTVRHotSpotInfoAtom), theHotSpotInfoPtr, NULL);			}		} else {			myErr = cannotFindAtomErr;		}	} else {		myErr = cannotFindAtomErr;	}	QTDisposeAtomContainer(myNodeInfo);	return(myErr);}////////////// QTVRUtils_GetStringFromAtom// Get the string data from the string atom having the specified ID in the specified atom container.//// We use a different strategy here, since we don't know the size of the string data in advance.////////////char *QTVRUtils_GetStringFromAtom (QTAtomContainer theContainer, QTAtom theParent, QTAtomID theID){	QTVRStringAtomPtr	myStringAtomPtr = NULL;	QTAtom				myNameAtom;	char				*myString = NULL; 	OSErr				myErr = noErr;	if (theContainer == NULL)		return(myString);			QTLockContainer(theContainer);		myNameAtom = QTFindChildByID(theContainer, theParent, kQTVRStringAtomType, theID, NULL);	if (myNameAtom != 0) {		myErr = QTGetAtomDataPtr(theContainer, myNameAtom, NULL, (Ptr *)&myStringAtomPtr);		if ((myErr == noErr) && (myStringAtomPtr != NULL)) {			UInt16		myLength;							myLength = EndianU16_BtoN(myStringAtomPtr->stringLength);			if (myLength > 0) {				myString = malloc(myLength + 1);				if (myString != NULL) {					memcpy(myString, myStringAtomPtr->theString, myLength);					myString[myLength] = '\0';				}			}					}	}		QTUnlockContainer(theContainer);	return(myString);}////////////// QTVRUtils_AddStr255ToAtomContainer// Add a Pascal string to the specified atom container; return (through theID) the ID of the new string atom.////////////OSErr QTVRUtils_AddStr255ToAtomContainer (QTAtomContainer theContainer, QTAtom theParent, Str255 theString, QTAtomID *theID){	OSErr					myErr = noErr;	if ((theContainer == NULL) || (theParent == 0) || (theID == NULL))		return(paramErr);			*theID = 0;				// initialize the returned atom ID	if (theString[0] != 0) {		QTAtom				myStringAtom;		UInt16				mySize;		QTVRStringAtomPtr	myStringAtomPtr = NULL;				mySize = sizeof(QTVRStringAtom) - 4 + theString[0];		myStringAtomPtr = (QTVRStringAtomPtr)NewPtrClear(mySize);				if (myStringAtomPtr != NULL) {			myStringAtomPtr->stringUsage = EndianU16_NtoB(1);			myStringAtomPtr->stringLength = EndianU16_NtoB(theString[0]);			BlockMove(theString + 1, myStringAtomPtr->theString, theString[0]);			myErr = QTInsertChild(theContainer, theParent, kQTVRStringAtomType, 0, 0, mySize, (Ptr)myStringAtomPtr, &myStringAtom);			DisposePtr((Ptr)myStringAtomPtr);						if (myErr == noErr)				QTGetAtomTypeAndID(theContainer, myStringAtom, NULL, theID);		}	}		return(myErr);}////////////// QTVRUtils_GetDefaultNodeID// Get the ID of the default node in a QTVR movie.////////////UInt32 QTVRUtils_GetDefaultNodeID (QTVRInstance theInstance){	QTVRWorldHeaderAtom	 	myVRWorldHeader;	UInt32					myNodeID = kQTVRCurrentNode;	OSErr					myErr = noErr;			myErr = QTVRUtils_GetVRWorldHeaderAtomData(theInstance, &myVRWorldHeader);	if (myErr == noErr)		myNodeID = EndianU32_BtoN(myVRWorldHeader.defaultNodeID);	return(myNodeID);}////////////// QTVRUtils_GetSceneFlags// Get the set of flags associated with the VR scene.// (Currently these flags are undefined, however.)////////////UInt32 QTVRUtils_GetSceneFlags (QTVRInstance theInstance){	QTVRWorldHeaderAtom	 	myVRWorldHeader;	UInt32					myFlags = 0L;	OSErr					myErr;			myErr = QTVRUtils_GetVRWorldHeaderAtomData(theInstance, &myVRWorldHeader);	if (myErr == noErr)		myFlags = EndianU32_BtoN(myVRWorldHeader.vrWorldFlags);	return(myFlags);}////////////// QTVRUtils_GetSceneName// Get the name of the VR scene.// The caller is responsible for disposing of the pointer returned by this function (by calling free).////////////char *QTVRUtils_GetSceneName (QTVRInstance theInstance){	QTVRWorldHeaderAtom	 		myVRWorldHeader;	char						*mySceneName = NULL;	OSErr						myErr = noErr;			myErr = QTVRUtils_GetVRWorldHeaderAtomData(theInstance, &myVRWorldHeader);	if (myErr == noErr) {		QTAtomID				myNameAtomID;				// get the atom ID of the name string atom		myNameAtomID = EndianU32_BtoN(myVRWorldHeader.nameAtomID);				if (myNameAtomID != 0) {			QTAtomContainer		myVRWorld;						// the string atom containing the name of the scene is a *sibling* of the VR world header atom			myErr = QTVRGetVRWorld(theInstance, &myVRWorld);			if (myErr == noErr)				mySceneName = QTVRUtils_GetStringFromAtom(myVRWorld, kParentAtomIsContainer, myNameAtomID);						QTDisposeAtomContainer(myVRWorld);		}	}	return(mySceneName);}////////////// QTVRUtils_GetNodeCount// Get the number of nodes in a QTVR movie.////////////UInt32 QTVRUtils_GetNodeCount (QTVRInstance theInstance){	QTAtomContainer			myVRWorld;	QTAtom					myNodeParentAtom;	UInt32					myNumNodes = 0;	OSErr					myErr = noErr;	// get the VR world	myErr = QTVRGetVRWorld(theInstance, &myVRWorld);	if (myErr != noErr)		return(myNumNodes);	// get the node parent atom, whose children contain info about all nodes in the scene	myNodeParentAtom = QTFindChildByIndex(myVRWorld, kParentAtomIsContainer, kQTVRNodeParentAtomType, 1, NULL);	if (myNodeParentAtom != 0) {		// now count the node ID children of the node parent atom, which is the number of nodes in the scene		myNumNodes = QTCountChildrenOfType(myVRWorld, myNodeParentAtom, kQTVRNodeIDAtomType);	}		QTDisposeAtomContainer(myVRWorld);		return(myNumNodes);}////////////// QTVRUtils_GetNodeType// Get the type of the node with the specified ID.//// NOTE: This function is redundant, given QTVRGetNodeType; it's included here for illustrative purposes only.////////////OSErr QTVRUtils_GetNodeType (QTVRInstance theInstance, UInt32 theNodeID, OSType *theNodeType){	QTVRNodeHeaderAtom		myNodeHeader;	OSErr					myErr = noErr;	// make sure we always return some meaningful value	*theNodeType = kQTVRUnknownType;		// get the node header atom data	myErr = QTVRUtils_GetNodeHeaderAtomData(theInstance, theNodeID, &myNodeHeader);	if (myErr == noErr)		*theNodeType = EndianU32_BtoN(myNodeHeader.nodeType);			return(myErr);}////////////// QTVRUtils_GetNodeName// Get the name of the node with the specified ID.// The caller is responsible for disposing of the pointer returned by this function (by calling free).////////////char *QTVRUtils_GetNodeName (QTVRInstance theInstance, UInt32 theNodeID){	QTVRNodeHeaderAtom		myNodeHeader;	char					*myNodeName = NULL;	OSErr					myErr = noErr;		myErr = QTVRUtils_GetNodeHeaderAtomData(theInstance, theNodeID, &myNodeHeader);	if (myErr == noErr) {		QTAtomID				myNameAtomID;				// get the atom ID of the name string atom		myNameAtomID = EndianU32_BtoN(myNodeHeader.nameAtomID);				if (myNameAtomID != 0) {			QTAtomContainer		myNodeInfo;						// the string atom containing the name of the node is a *sibling* of the node information atom			myErr = QTVRGetNodeInfo(theInstance, theNodeID, &myNodeInfo);			if (myErr == noErr)				myNodeName = QTVRUtils_GetStringFromAtom(myNodeInfo, kParentAtomIsContainer, myNameAtomID);			QTDisposeAtomContainer(myNodeInfo);		}	}		return(myNodeName);}////////////// QTVRUtils_GetNodeComment// Get the comment for the node with the specified ID.// The caller is responsible for disposing of the pointer returned by this function (by calling free).////////////char *QTVRUtils_GetNodeComment (QTVRInstance theInstance, UInt32 theNodeID){	QTVRNodeHeaderAtom		myNodeHeader;	char					*myNodeCmt = NULL;	OSErr					myErr = noErr;		myErr = QTVRUtils_GetNodeHeaderAtomData(theInstance, theNodeID, &myNodeHeader);	if (myErr == noErr) {		QTAtomID				myCmtAtomID;				// get the atom ID of the comment string atom		myCmtAtomID = EndianU32_BtoN(myNodeHeader.commentAtomID);				if (myCmtAtomID != 0) {			QTAtomContainer		myNodeInfo;						// the string atom containing the comment for the node is a *sibling* of the node information atom			myErr = QTVRGetNodeInfo(theInstance, theNodeID, &myNodeInfo);			if (myErr == noErr)				myNodeCmt = QTVRUtils_GetStringFromAtom(myNodeInfo, kParentAtomIsContainer, myCmtAtomID);			QTDisposeAtomContainer(myNodeInfo);		}	}		return(myNodeCmt);}////////////// QTVRUtils_GetHotSpotCount// Return the number of hot spots in the node with specified ID,// and fill the specified handle with a list of the hot spot IDs.//// If theHotSpotIDs == NULL on entry, do not pass back the list of IDs.//// WARNING: This routine determines the number of hot spots by counting// the hot spot atoms in a hot spot parent atom; this might not be// the same as counting the number of regions in the hot spot image track.// Sigh.////////////UInt32 QTVRUtils_GetHotSpotCount (QTVRInstance theInstance, UInt32 theNodeID, Handle theHotSpotIDs){	QTAtomContainer			myNodeInfo;	QTAtom					myHSParentAtom = 0;	UInt32					myNumHotSpots = 0;	OSErr					myErr = noErr;		// get the node information atom container for the current node	myErr = QTVRGetNodeInfo(theInstance, theNodeID, &myNodeInfo);		// get the hot spot parent atom	if (myErr == noErr)		myHSParentAtom = QTFindChildByID(myNodeInfo, kParentAtomIsContainer, kQTVRHotSpotParentAtomType, 1, NULL);			if (myHSParentAtom != 0) {		SignedByte			myHState;		Size				mySize;		// get the number of hot spots in the current node		myNumHotSpots = QTCountChildrenOfType(myNodeInfo, myHSParentAtom, kQTVRHotSpotAtomType);				// now pass back a list of the hot spot IDs;		// if theHotSpotIDs is NULL on entry, we assume the caller doesn't want this information		if (theHotSpotIDs != NULL) {					// unlock the handle, if it's locked (so that we can resize it)			myHState = HGetState(theHotSpotIDs);			if (myHState & 0x80)			// 0x80 == the block-is-locked bit in the SignedByte returned by HGetState				HUnlock(theHotSpotIDs);			// resize the handle to the appropriate size			mySize = sizeof(UInt32) * myNumHotSpots;			SetHandleSize(theHotSpotIDs, mySize);						// restore the original handle state			HSetState(theHotSpotIDs, myHState);						// make sure we actually did resize the handle			if (GetHandleSize(theHotSpotIDs) == mySize) {				short			myIndex;				QTAtom			myAtom;				QTAtomID		myID;				UInt32			*myIDPtr;								myIDPtr = (UInt32 *)*theHotSpotIDs;				// loop thru all the hot spots to get their IDs				for (myIndex = 1; myIndex <= (short)myNumHotSpots; myIndex++) {					myAtom = QTFindChildByIndex(myNodeInfo, myHSParentAtom, kQTVRHotSpotAtomType, myIndex, &myID);					myIDPtr[myIndex - 1] = (UInt32)myID;				}			}		}	}		QTDisposeAtomContainer(myNodeInfo);	return(myNumHotSpots);}////////////// QTVRUtils_GetHotSpotIDByIndex// Return the hot spot ID having the specified index in the list of hot spot IDs returned by QTVRUtils_GetHotSpotCount,// or kQTVRUtils_InvalidHotSpotID if no such hot spot exists.////////////UInt32 QTVRUtils_GetHotSpotIDByIndex (QTVRInstance theInstance, Handle theHotSpotIDs, UInt32 theIndex){	Size			mySize;	UInt32			myID = kQTVRUtils_InvalidHotSpotID;	UInt32			*myIDPtr;					// make sure the instance and hot spot list are non-NULL	if ((theInstance == NULL) || (theHotSpotIDs == NULL))		return(myID);		// make sure that the index is valid		mySize = GetHandleSize(theHotSpotIDs);	if (theIndex >= (mySize / sizeof(UInt32)))		return(myID);	myIDPtr = (UInt32 *)*theHotSpotIDs;	myID = myIDPtr[theIndex];	return(myID);}////////////// QTVRUtils_GetHotSpotType// Return the type of the hot spot having the specified hot spot ID in the specified node.//// NOTE: This function is semi-redundant, given QTVRGetHotSpotType; it's included here for illustrative purposes only.// (Note, however, that QTVRGetHotSpotType returns types only for hot spots in the current node; here we do any node!)////////////OSErr QTVRUtils_GetHotSpotType (QTVRInstance theInstance, UInt32 theNodeID, UInt32 theHotSpotID, OSType *theHotSpotType){	QTVRHotSpotInfoAtom		myHotSpotAtomData;	OSErr					myErr = noErr;		// make sure we always return some meaningful value	*theHotSpotType = kQTVRHotSpotUndefinedType;		// get the hot spot information atom data	myErr = QTVRUtils_GetHotSpotAtomData(theInstance, theNodeID, theHotSpotID, &myHotSpotAtomData);	if (myErr == noErr)		*theHotSpotType = EndianU32_BtoN(myHotSpotAtomData.hotSpotType);		// return the hot spot type		return(myErr);}////////////// QTVRUtils_GetHotSpotName// Return the name of the hot spot having the specified hot spot ID in the specified node.// The caller is responsible for disposing of the pointer returned by this function (by calling free).////////////char *QTVRUtils_GetHotSpotName (QTVRInstance theInstance, UInt32 theNodeID, UInt32 theHotSpotID){	QTVRHotSpotInfoAtom		myHotSpotAtomData;	char					*myHotSpotName = NULL;	OSErr					myErr = noErr;		// get the hot spot information atom data	myErr = QTVRUtils_GetHotSpotAtomData(theInstance, theNodeID, theHotSpotID, &myHotSpotAtomData);	if (myErr == noErr) {		QTAtomID				myNameAtomID;				// get the atom ID of the name string atom		myNameAtomID = EndianU32_BtoN(myHotSpotAtomData.nameAtomID);				if (myNameAtomID != 0) {			QTAtomContainer		myNodeInfo;			QTAtom				myHSParentAtom;			QTAtom				myHSAtom;			QTAtom				myNameAtom = 0;						// version 2.0 documentation says that the hot spot name is contained in a string atom			// that is a sibling of the hot spot atom (that is, a child of the hot spot parent atom);			// some other documents indicate that a string atom is always a sibling of the atom that			// contains the reference (in this case, a sibling of the hot spot information atom, and			// hence a child of the hot spot atom); we will look first in the hot spot atom and then			// in the hot spot parent atom. The version 2.1 documentation corrects the earlier error.			// Mea culpa!			// get the hot spot parent atom and the hot spot atom			myErr = QTVRGetNodeInfo(theInstance, theNodeID, &myNodeInfo);			if (myErr == noErr) {				myHSParentAtom = QTFindChildByID(myNodeInfo, kParentAtomIsContainer, kQTVRHotSpotParentAtomType, 1, NULL);				if (myHSParentAtom != 0) {					myHSAtom = QTFindChildByID(myNodeInfo, myHSParentAtom, kQTVRHotSpotAtomType, theHotSpotID, NULL);					if (myHSAtom != 0) {						QTAtom	myParentAtom;												// look for a string atom that is a child of the hot spot atom						myParentAtom = myHSAtom;						myNameAtom = QTFindChildByID(myNodeInfo, myParentAtom, kQTVRStringAtomType, theHotSpotID, NULL);						if (myNameAtom == 0) {							// no such atom in the hot spot atom; look in the hot spot parent atom							myParentAtom = myHSParentAtom;							myNameAtom = QTFindChildByID(myNodeInfo, myParentAtom, kQTVRStringAtomType, theHotSpotID, NULL);						}												if (myNameAtom != 0)							myHotSpotName = QTVRUtils_GetStringFromAtom(myNodeInfo, myParentAtom, myNameAtomID);					}				}			}						QTDisposeAtomContainer(myNodeInfo);		}	}	return(myHotSpotName);}/////////////////////////////////////////////////////////////////////////////////////////////////////////////// Miscellaneous utilities./////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// QTVRUtils_Point3DToPanAngle// Return the QTVR pan angle for a given QD3D point.// //////////float QTVRUtils_Point3DToPanAngle (float theX, float theY, float theZ){#pragma unused(theY)	float	myPan;		if (theZ != 0.0) {		// note that atan always returns angles in the range -�/2 to �/2		myPan = atan(theX / theZ);		myPan = (theZ > 0) ? myPan + kVRPi : myPan;	} else {		myPan = (theX > 0) ? kVR3PiOver2 : kVRPiOver2;	}		// make sure myPan is positive	while (myPan < 0.0)		myPan += kVR2Pi;	return(myPan);}////////////// QTVRUtils_Point3DToTiltAngle// Return the QTVR tilt angle for a given QD3D point.// //////////float QTVRUtils_Point3DToTiltAngle (float theX, float theY, float theZ){	float			myTilt;	float			myDistance;	TQ3Point3D		myPoint;		myPoint.x = theX;	myPoint.y = theY;	myPoint.z = theZ;		myDistance = QTVRUtils_GetDistance(myPoint);	if (myDistance != 0.0)		myTilt = asin(theY / myDistance); 	else		myTilt = 0.0;		return(myTilt);}////////////// QTVRUtils_PanAngleToPoint3D// Return a unit vector for a given QTVR pan angle.//// The pan angle is expected to be in radians.// //////////void QTVRUtils_PanAngleToPoint3D (float thePanAngle, float *theX, float *theZ){	// sin and cos are continuous functions and have no limits on input range;	// output range is always -1..1	*theX = sin(thePanAngle);	*theZ = -cos(thePanAngle);}/////////////////////////////////////////////////////////////////////////////////////////////////////////////// Node callback utilities.//// Use these to obtain standard behaviors when entering or exiting nodes./////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// QTVRUtils_StandardEnteringNodeProc// A standard procedure for entering a new node.//// This function performs actions that many applications will want done when entering a new node://	* display back button only if multinode movie//	* display show-hot-spot button only if there are hotspots//	* display the translate button only for object nodes that can translate//	* (this space for rent)////////////PASCAL_RTN OSErr QTVRUtils_StandardEnteringNodeProc (QTVRInstance theInstance, long theNodeID, MovieController theMC){#pragma unused(theNodeID)	OSErr		myErr = noErr;	if ((theInstance == NULL) || (theMC == NULL))		return(paramErr);			/////// 	// all nodes	/////// 			// display the back button only if it's a multinode movie	if (QTVRUtils_IsMultiNode(theInstance))		QTUtils_ShowControllerButton(theMC, kQTUtilsBackButton);	else		QTUtils_HideControllerButton(theMC, kQTUtilsBackButton);	// display the show-hot-spot button only if there are hotspots in the node	if (QTVRUtils_IsHotSpotInNode(theInstance))		QTUtils_ShowControllerButton(theMC, kQTUtilsHotSpotButton);	else		QTUtils_HideControllerButton(theMC, kQTUtilsHotSpotButton);	/////// 	// panoramic nodes	/////// 		if (QTVRUtils_IsPanoNode(theInstance)) {			// hide the translate button		QTUtils_HideControllerButton(theMC, kQTUtilsTranslateButton);			} else {		/////// 	// object nodes	/////// 				// show the translate button, but only if translation is available		if (QTVRUtils_IsTranslateAvailable(theInstance))			QTUtils_ShowControllerButton(theMC, kQTUtilsTranslateButton);		else			QTUtils_HideControllerButton(theMC, kQTUtilsTranslateButton);	}		return(myErr);}////////////// QTVRUtils_StandardLeavingNodeProc// A standard procedure for leaving a node.// This function performs actions that many applications will want done when leaving a node://	* (this space for rent)//// We assume that when this procedure is called, the application has decided NOT to cancel the move;// accordingly, we always return false in theCancel.////////////PASCAL_RTN OSErr QTVRUtils_StandardLeavingNodeProc (QTVRInstance theInstance, long fromNodeID, long toNodeID, Boolean *theCancel, MovieController theMC){#pragma unused(fromNodeID, toNodeID)	OSErr		myErr = noErr;	if ((theInstance == NULL) || (theMC == NULL))		return(paramErr);		// nothing yet....	*theCancel = false;	return(myErr);}