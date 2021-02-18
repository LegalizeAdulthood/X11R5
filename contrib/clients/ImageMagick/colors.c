/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                  CCCC   OOO   L       OOO   RRRR    SSSSS                   %
%                 C      O   O  L      O   O  R   R   SS                      %
%                 C      O   O  L      O   O  RRRR     SSS                    %
%                 C      O   O  L      O   O  R R        SS                   %
%                  CCCC   OOO   LLLLL   OOO   R  R    SSSSS                   %
%                                                                             %
%                                                                             %
%                       Count the Colors in an Image                          %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                              July 1991                                      %
%                                                                             %
%                                                                             %
%  Copyright 1991 E. I. du Pont de Nemours & Company                          %
%                                                                             %
%  Permission to use, copy, modify, distribute, and sell this software and    %
%  its documentation for any purpose is hereby granted without fee,           %
%  provided that the above Copyright notice appear in all copies and that     %
%  both that Copyright notice and this permission notice appear in            %
%  supporting documentation, and that the name of E. I. du Pont de Nemours    %
%  & Company not be used in advertising or publicity pertaining to            %
%  distribution of the software without specific, written prior               %
%  permission.  E. I. du Pont de Nemours & Company makes no representations   %
%  about the suitability of this software for any purpose.  It is provided    %
%  "as is" without express or implied warranty.                               %
%                                                                             %
%  E. I. du Pont de Nemours & Company disclaims all warranties with regard    %
%  to this software, including all implied warranties of merchantability      %
%  and fitness, in no event shall E. I. du Pont de Nemours & Company be       %
%  liable for any special, indirect or consequential damages or any           %
%  damages whatsoever resulting from loss of use, data or profits, whether    %
%  in an action of contract, negligence or other tortious action, arising     %
%  out of or in connection with the use or performance of this software.      %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "display.h"
#include "image.h"
/*
  Define declarations.
*/
#define MaxTreeDepth  8  /* Log2(MaxRgb) */
#define NodesInAList  2047

/*
  Structures.
*/
typedef struct _Node
{
  struct _Node
    *child[8];

  unsigned char
    mid_red,
    mid_green,
    mid_blue;
} Node;

typedef struct _Nodes
{
  Node
    nodes[NodesInAList];

  struct _Nodes
    *next;
} Nodes;

typedef struct _Cube
{
  Node
    *root,
    *leaf;

  unsigned int
    colors;

  unsigned int
    free_nodes;

  Node
    *node;

  Nodes
    *node_list;
} Cube;

/*
  Global variables.
*/
static Cube
  cube;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I n i t i a l i z e N o d e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function InitializeNode allocates memory for a new node in the color cube
%  tree and presets all fields to zero.
%
%  The format of the InitializeNode routine is: 
%
%      node=InitializeNode(mid_red,mid_green,mid_blue)
%
%  A description of each parameter follows.
%
%    o mid_red: Specifies the mid point of the red axis for this node.
%
%    o mid_green: Specifies the mid point of the green axis for this node.
%
%    o mid_blue: Specifies the mid point of the blue axis for this node.
%
%
*/
static Node *InitializeNode(mid_red,mid_green,mid_blue)
register unsigned char
  mid_red,
  mid_green,
  mid_blue;
{
  register int
    i;

  register Node
    *node;

  if (cube.free_nodes == 0)
    {
      register Nodes
        *nodes;

      /*
        Allocate a new nodes of nodes.
      */
      nodes=(Nodes *) malloc(sizeof(Nodes));
      if (nodes == (Nodes *) NULL)
        return((Node *) NULL);
      nodes->next=cube.node_list;
      cube.node_list=nodes;
      cube.node=nodes->nodes;
      cube.free_nodes=NodesInAList;
    }
  cube.free_nodes--;
  node=cube.node++;
  for (i=0; i < 8; i++)
    node->child[i]=(Node *) NULL;
  node->mid_red=mid_red;
  node->mid_green=mid_green;
  node->mid_blue=mid_blue;
  return(node);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  N u m b e r C o l o r s                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function NumberColors returns the number of unique colors in an image.
%
%  The format of the NumberColors routine is: 
%
%      number_colors=NumberColors(image)
%
%  A description of each parameter follows.
%
%    o image: The address of a byte (8 bits) array of run-length
%      encoded pixel data of your source image.  The sum of the
%      run-length counts in the source image must be equal to or exceed
%      the number of pixels.
%
%
*/
unsigned int NumberColors(image)
Image
  *image;
{
  Nodes
    *nodes;

  register RunlengthPacket
    *p;

  register int
    i;

  register Node
    *node;

  register unsigned char
    bisect,
    id;

  register unsigned int
    level;

  /*
    Initialize color description tree.
  */
  cube.node_list=(Nodes *) NULL;
  cube.colors=0;
  cube.free_nodes=0;
  cube.root=InitializeNode(MaxRgb >> 1,MaxRgb >> 1,MaxRgb >> 1);
  cube.leaf=InitializeNode(0,0,0);
  if ((cube.root == (Node *) NULL) || (cube.leaf == (Node *) NULL))
    {
      Warning("unable to count colors","memory allocation failed");
      return(0);
    }
  p=image->pixels;
  for (i=0; i < image->packets; i++)
  {
    /*
      Start at the root and proceed level by level.
    */
    node=cube.root;
    for (level=1; level < MaxTreeDepth; level++)
    {
      id=(p->red > node->mid_red) | (p->green > node->mid_green) << 1 |
        (p->blue > node->mid_blue) << 2;
      if (node->child[id] == (Node *) NULL)
        {
          bisect=(1 << (MaxTreeDepth-level)) >> 1;
          node->child[id]=InitializeNode(
            node->mid_red+(id & 1 ? bisect : -bisect),
            node->mid_green+(id & 2 ? bisect : -bisect),
            node->mid_blue+(id & 4 ? bisect : -bisect));
          if (node->child[id] == (Node *) NULL)
            {
              Warning("unable to count colors","memory allocation failed");
              return(0);
            }
        }
      node=node->child[id];
    }
    id=(p->red > node->mid_red) | (p->green > node->mid_green) << 1 |
      (p->blue > node->mid_blue) << 2;
    if (node->child[id] == (Node *) NULL)
      {
        node->child[id]=cube.leaf;
        cube.colors++;
      }
    p++;
  }
  /*
    Release color cube tree storage.
  */
  do
  {
    nodes=cube.node_list->next;
    (void) free((char *) cube.node_list);
    cube.node_list=nodes;
  }
  while (cube.node_list != (Nodes *) NULL);
  return(cube.colors);
}
