// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2003-2008 - TortoiseSVN

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#include "StdAfx.h"
#include "CollapseTreeDownward.h"
#include "VisibleGraphNode.h"
#include "GraphNodeState.h"

// construction

CCollapseTreeDownward::CCollapseTreeDownward 
    ( CRevisionGraphOptionList& list
    , CGraphNodeStates* nodeStates)
    : inherited (list)
    , nodeStates (nodeStates)
{
}

// implement IModificationOption

void CCollapseTreeDownward::Apply (CVisibleGraph* graph, CVisibleGraphNode* node)
{
    // short-cut: most nodes won't have a state info

    DWORD state = nodeStates->GetFlags (node->GetBase());
    if (state == 0)
        return;

    // cut at this tree node, if requested by the node state

    if (state & CGraphNodeStates::COLLAPSED_BELOW)
    {
        CVisibleGraphNode* next = node->GetNext();
        if (next)
            next->DropSubTree (graph);
    }

    if (state & CGraphNodeStates::COLLAPSED_RIGHT)
    {
        while (node->GetFirstCopyTarget() != NULL)
            node->GetFirstCopyTarget()->value()->DropSubTree (graph);
    }
}
