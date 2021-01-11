/*
LightsOff:
A clone of the Tim Horton's LightsOff program written in C, adding variable
board sizes, undo and clear movements and a solver.

Copyright 2016-2021, Javier Burguete Tolosa.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
        this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY AUTHORS ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

/**
 * \file game.c
 * \brief Source file of the lights off game.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2016-2021, Javier Burguete Tolosa.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <glib.h>
#include "config.h"
#include "game.h"

unsigned int nrows;             ///< Rows number.
unsigned int ncolumns;          ///< Columns number.
unsigned int nsquares;          ///< Squares number.
unsigned int level = DEFAULT_LEVEL;     ///< Game level.

uint64_t status;                ///< Current game status.
int nmovements;                 ///< Movements number of the optimal solution.
///> Array of movements of the optimal solution.
unsigned int movement[N_MAX_SQUARES];
///> Array of possible movements codified in bit chains.
uint64_t movements_array[N_MAX_SQUARES];

/**
 * Function to generate the array of movements.
 */
static inline void
generate_movements ()
{
  unsigned int i, j, top, left, centre, right, bottom;

  // left top corner
  centre = 0;
  right = 1;
  bottom = ncolumns;
  movements_array[centre] = (1L << centre) | (1L << right) | (1L << bottom);

  // centre top squares
  while (++centre < ncolumns - 1)
    {
      right = centre + 1;
      left = centre - 1;
      bottom = centre + ncolumns;
      movements_array[centre] = (1L << centre) | (1L << right) | (1L << left)
        | (1L << bottom);
    }

  // right top corner
  left = centre - 1;
  bottom = centre + ncolumns;
  movements_array[centre] = (1L << centre) | (1L << left) | (1L << bottom);

  // centre rows
  for (i = 1; i < nrows - 1; ++i)
    {

      // left centre squares
      centre = i * ncolumns;
      right = centre + 1;
      bottom = centre + ncolumns;
      top = centre - ncolumns;
      movements_array[centre] =
        (1L << centre) | (1L << right) | (1L << bottom) | (1L << top);

      // centre centre squares
      for (j = 1; j < ncolumns - 1; ++j)
        {
          ++centre;
          right = centre + 1;
          left = centre - 1;
          bottom = centre + ncolumns;
          top = centre - ncolumns;
          movements_array[centre] = (1L << centre) | (1L << right)
            | (1L << left) | (1L << bottom) | (1L << top);
        }

      // right centre squares
      ++centre;
      left = centre - 1;
      bottom = centre + ncolumns;
      top = centre - ncolumns;
      movements_array[centre] = (1L << centre) | (1L << left) | (1L << bottom)
        | (1L << top);
    }

  // left bottom corner
  centre = i * ncolumns;
  right = centre + 1;
  top = centre - ncolumns;
  movements_array[centre] = (1L << centre) | (1L << right) | (1L << top);

  // centre bottom squares
  for (j = 1; j < ncolumns - 1; ++j)
    {
      ++centre;
      right = centre + 1;
      left = centre - 1;
      top = centre - ncolumns;
      movements_array[centre] = (1L << centre) | (1L << right) | (1L << left)
        | (1L << top);
    }

  // right bottom corner
  ++centre;
  left = centre - 1;
  top = centre - ncolumns;
  movements_array[centre] = (1L << centre) | (1L << left) | (1L << top);
}

/**
 * Function to init a new game.
 */
void
game_init ()
{
  nsquares = nrows * ncolumns;
  generate_movements ();
  status = 0L;
}

/**
 * Function to generate a new game.
 */
void
game_new ()
{
  GRand *rand;
  uint64_t m;
  unsigned int i, j;
  game_init ();
  rand = g_rand_new ();
  m = 0L;
  for (i = 0; i < level; ++i)
    {
      do
        j = g_rand_int_range (rand, 0, nsquares);
      while (!((~m) & (1L << j)));
      m |= (1L << j);
      move (&status, j);
    }
  g_rand_free (rand);
}

/**
 * Function to do one movement.
 *
 * \return type of movement hunting the light.
 */
static inline int
hunt (uint64_t * status)        ///< bits chain codifying the game status.
{
  unsigned int i, n;
  n = nsquares - ncolumns;
  for (i = 0; i < n; ++i)
    {
      if (*status & (1L << i))
        {
          move (status, i + ncolumns);
          break;
        }
    }
  if (i == n)
    return 0;
  return i + ncolumns;
}

/**
 * Function to elliminate lights by the hunt algorithm.
 *
 * \return on succes: number of movements; on failure: 0.
 */
static unsigned int
play_hunt (uint64_t status,     ///< bits chain codifying the game status.
           unsigned int depth,  ///< number of past movements.
           unsigned int *movement)      ///< array of movements.
{
  uint64_t j, m;
  unsigned int i, n;
  n = nsquares - ncolumns;
  m = (1L << n) - 1;
  j = status;
  do
    {
      ++depth;
      i = hunt (&j);
      movement[depth - 1] = i;
      if (!j)
        return depth;
    }
  while (j & m);
  return 0;
}

/**
 * Function to solve the game recursively.
 */
static void
play_recursive (uint64_t status,
///< bits chain codifying the game status.
                unsigned int depth,     ///< number of past movements.
                unsigned int ii,
///< index of the minimum allowed movement.
                unsigned int *m,        ///< array of current movements.
                unsigned int *kmax,
///< number of movements of the current best play.
                unsigned int *movement) ///< array of optimal movements.
{
  uint64_t j;
  unsigned int i, k;

  // Checking lights hunt doing one movement at top
  for (i = ii; i < ncolumns; ++i)
    {
      j = status;
      move (&j, i);
      m[depth - 1] = i;
      if (!j && depth < *kmax)
        {
          // Saving a better solution
          *kmax = depth;
          memcpy (movement, m, *kmax * sizeof (unsigned int));
        }
      k = play_hunt (j, depth, m);
      if (k && k < *kmax)
        {
          // Saving a better solution
          *kmax = k;
          memcpy (movement, m, *kmax * sizeof (unsigned int));
        }
      // Trying another movement at top
      if (depth < ncolumns)
        play_recursive (j, depth + 1, i + 1, m, kmax, movement);
    }
}

/**
 * Function to search the optimal play to elliminate the lights.
 *
 * \return on succes: number of movements; on failure: -1.
 */
int
play ()
{
  unsigned int m[N_MAX_SQUARES];
  unsigned int k, kmax;

  // Checking obvious solution
  if (!status)
    return 0;

  // Setting an upper limit of movements number
  kmax = nsquares + 1;

  // Checking lights hunt without movements at top
  k = play_hunt (status, 0, m);
  if (k)
    {
      // Saving a solution
      kmax = k;
      memcpy (movement, m, kmax * sizeof (unsigned int));
    }

  // Traying lights hunt making movement combinations at top
  play_recursive (status, 1, 0, m, &kmax, movement);

  // Checking if the solution exists
  if (kmax <= nsquares)
    return kmax;

  // Unable to find a solution
  return -1;
}
