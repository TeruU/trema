/*
 * Author: Yasunobu Chiba
 *
 * Copyright (C) 2011 NEC Corporation
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

#include <assert.h>
#include <inttypes.h>
#include "libpath.h"
#include "trema.h"

void
dump_hop( const hop *h ) {
  info( "Hop: datapath_id = %#" PRIx64 ", in_port = %u, out_port = %u, extra_actions = %p.",
        h->datapath_id, h->in_port, h->out_port, h->extra_actions );
}


void
dump_path( const path *p ) {
  char match_string[ 256 ];
  match_to_string( &p->match, match_string, sizeof( match_string ) );

  info( "Path: match = [%s], priority = %u, idle_timeout = %u, hard_timeout = %u, n_hops = %d, hops = %p.",
        match_string, p->priority, p->idle_timeout, p->hard_timeout, p->n_hops, p->hops );
  if ( p->n_hops > 0 && p->hops != NULL ) {
    list_element *e = p->hops;
    while ( e != NULL ) {
      dump_hop( e->data );
      e = e->next;
    }
  }
}


const char *
status_to_string( int status ) {
  switch ( status ) {
    case SETUP_SUCCEEDED:
      return "succeeded";
    break;
    case SETUP_CONFLICTED_ENTRY:
      return "conflicted entry";
    break;
    case SETUP_SWITCH_ERROR:
      return "switch error";
    break;
    default:
    break;
  }

  return "undefined";
}


const char *
reason_to_string( int reason ) {
  switch ( reason ) {
    case TEARDOWN_TIMEOUT:
      return "timeout";
    break;
    case TEARDOWN_MANUALLY_REQUESTED:
      return "manually requested";
    break;
    default:
    break;
  }

  return "undefined";
}


static void
handle_setup( int status, const path *p, void *user_data ) {
  info( "**** Path setup completed ( status = %s, user_data = %p ) *** ",
        status_to_string( status ), user_data );
  dump_path( p );

}


static void
handle_teardown( int reason, const path *p, void *user_data ) {
  info( "**** Path teardown completed ( reason = %s, user_data = %p ) *** ",
        reason_to_string( reason ), user_data );
  dump_path( p );
  stop_trema();
}


static void
install_flow_entry( void *user_data ) {
  UNUSED( user_data );

  struct ofp_match match;
  memset( &match, 0, sizeof( struct ofp_match ) );
  match.wildcards = OFPFW_ALL;

  uint16_t priority = UINT16_MAX;
  uint16_t idle_timeout = 10;
  uint16_t hard_timeout = 20;

  path *p = create_path( match, priority, idle_timeout, hard_timeout );
  assert( p != NULL );
  hop *h1 = create_hop( 0x1, 1, 2, NULL );
  assert( h1 != NULL );
  append_hop_to_path( p, h1 );
  hop *h2 = create_hop( 0x2, 2, 1, NULL );
  assert( h2 != NULL );
  append_hop_to_path( p, h2 );

  info( "**** Setting up a path *** " );
  dump_path( p );

  bool ret = setup_path( p, handle_setup, NULL, handle_teardown, NULL );
  assert( ret == true );

  delete_path( p );
}


int
main( int argc, char *argv[] ) {
  // Initialize the Trema world
  init_trema( &argc, &argv );

  // Init path management library (libpath)
  init_path();

  // Set timer event to setup a path
  struct itimerspec spec;
  memset( &spec, 0, sizeof( struct itimerspec ) );
  spec.it_value.tv_sec = 1;
  add_timer_event_callback( &spec, install_flow_entry, NULL );

  // Main loop
  start_trema();

  // Finalize path management library (libpath)
  finalize_path();

  return 0;
}


/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
