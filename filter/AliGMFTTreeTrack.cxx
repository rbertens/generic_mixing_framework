/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* example track class which can stored ina ttree
 * see macros in PWGCF/FLOW/Documentation/examples/manual/ttree/macros for
 * usage info
 * author: redmer alexander bertens (rbertnens@cern.ch)
 */

#include "AliGMFTTreeTrack.h"

ClassImp(AliGMFTTreeTrack);

AliGMFTTreeTrack::AliGMFTTreeTrack() : TObject(),
    fPt          (-1),
    fEta         (-999),
    fPhi         (-999),
    fCharge      (-9999999),
    fUsed        (kFALSE)
{
  // default constructor
}
 
AliGMFTTreeTrack::~AliGMFTTreeTrack(){}
