/*
 * Unit tests for buf functions and macros.
 *
 * Copyright (C) 2008-2012 NEC Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include "libpath.h"
#include "checks.h"
#include "cmockery_trema.h"
#include "openflow.h"
#include "packet_info.h"
#include "trema.h"


/********************************************************************************
 * Tests.
 ********************************************************************************/

static void
test_create_hop_succeeds() {
	  hop *h1 = create_hop( 0x1, 1, 2, NULL );
	  assert_true(h1 != NULL);
}



/********************************************************************************
 * Run tests.
 ********************************************************************************/

int
main() {
  const UnitTest tests[] = {
    unit_test( test_create_hop_succeeds ),
  };
  //setup_leak_detector();
  return run_tests( tests );
}
