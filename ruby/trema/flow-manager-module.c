/*
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

#include <string.h>
#include <stdbool.h>
#include "trema.h"
#include "ruby.h"
#include "libpath.h"
#include "flow-manager-path.h"
#include "match.h"

#define DEBUG
#ifdef DEBUG
#define debug(...) {printf("%s(%d):", __func__, __LINE__); printf(__VA_ARGS__);}
#else
#define debug(...) 1 ? (void) 0 : printf
#endif

VALUE mFlowManager;

static const char *
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

static const char *
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
dump_hop( const hop *h ) {
  info( "Hop: datapath_id = %#" PRIx64 ", in_port = %u, out_port = %u, extra_actions = %p.",
        h->datapath_id, h->in_port, h->out_port, h->extra_actions );
}

static void
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

static void
handle_setup( int status, const path *p, void *controller ) {
  info( "**** Path setup completed ( status = %s)*** ", status_to_string( status ));
  dump_path( p );

	VALUE nullMatch = rb_funcall(cMatch, rb_intern("new"), 0);
	VALUE obj = rb_funcall( cPath, rb_intern( "new" ), 1, nullMatch );

	path *_path;
	Data_Get_Struct( obj, path, _path );

	_path->hard_timeout = p->hard_timeout;
	_path->hops = p->hops;
	_path->idle_timeout = p->idle_timeout;
	_path->match = p->match;
	_path->n_hops = p->n_hops;
	_path->priority = p->priority;

  printf("controller : %p\n", controller);
  if ( rb_respond_to( ( VALUE ) controller, rb_intern( "flow_manager_setup_reply" ) ) == Qtrue ) {
    rb_funcall( ( VALUE ) controller, rb_intern( "flow_manager_setup_reply" ), 2, rb_str_new2(status_to_string( status )), obj);
  }
}

static void
handle_teardown( int reason, const path *p, void *controller ) {
  info("Path teardown completed");
  info( "***( reason = %s)*** ", reason_to_string( reason ));
  dump_path( p );

	VALUE nullMatch = rb_funcall(cMatch, rb_intern("new"), 0);
	VALUE obj = rb_funcall( cPath, rb_intern( "new" ), 1, nullMatch );

	path *_path;
	Data_Get_Struct( obj, path, _path );

	_path->hard_timeout = p->hard_timeout;
	_path->hops = p->hops;
	_path->idle_timeout = p->idle_timeout;
	_path->match = p->match;
	_path->n_hops = p->n_hops;
	_path->priority = p->priority;

  if ( rb_respond_to( ( VALUE ) controller, rb_intern( "flow_manager_teardown_reply" ) ) == Qtrue ) {
    rb_funcall( ( VALUE ) controller, rb_intern( "flow_manager_teardown_reply" ), 2, rb_str_new2(reason_to_string( reason )), obj);
  }
  //stop_trema();
}

static VALUE flow_manager_teardown(VALUE self, VALUE in_datapath_id ,VALUE flow_manager_path)
{
	debug("start");
	path *p;
	Data_Get_Struct(flow_manager_path, path, p);
	dump_path( p );

	uint64_t _in_datapath_id = NUM2INT(in_datapath_id);
	struct ofp_match _match = p->match;
	uint16_t _priority = p->priority;

	bool ret = teardown_path(_in_datapath_id, _match, _priority );
	printf("b : %d\n", ret);

	if(ret == 1)
	{
		debug("end with true");
		return Qtrue;
	}
	else
	{
		debug("end with false");
		return Qfalse;
	}
}

static VALUE flow_manager_lookup(VALUE self, VALUE datapath_id, VALUE match, VALUE priority)
{
	debug("start\n");

	struct ofp_match *_match;
	uint64_t _datapath_id = NUM2INT(datapath_id);
	uint16_t _proirity = NUM2INT(priority);
	Data_Get_Struct(match, struct ofp_match, _match );

	path *p = lookup_path(_datapath_id, *_match, _proirity);

	VALUE match2 = rb_funcall(cMatch, rb_intern("new"), 0);
	VALUE obj = rb_funcall( cPath, rb_intern( "new" ), 1, match2 );

	path *_path;
	Data_Get_Struct( obj, path, _path );

	dump_path(p);
	_path->hard_timeout = p->hard_timeout;
	_path->hops = p->hops;
	_path->idle_timeout = p->idle_timeout;
	_path->match = p->match;
	_path->n_hops = p->n_hops;
	_path->priority = p->priority;

	debug("end\n");
	return obj;
}

static VALUE flow_manager_setup(VALUE self, VALUE flow_manager_path, VALUE controller)
{
	debug("start\n");

	path *p;
	Data_Get_Struct(flow_manager_path, path, p);

	dump_path(p);
	bool ret = setup_path( p, handle_setup, controller, handle_teardown, controller );

	debug("ret : %d\n", ret);

	if(ret == true)
	{
		debug("end with true \n");
		return Qtrue;
	}
	else
	{
		debug("end with false \n");
		return Qfalse;
	}
}

static VALUE flow_manager_append_hop_to_path(VALUE self, VALUE flow_manager_path, VALUE flow_manager_hop)
{
	debug("start\n");

	path *p;
	hop *h;
	Data_Get_Struct(flow_manager_path, path, p);
	Data_Get_Struct(flow_manager_hop, hop, h);

	append_hop_to_path(p, h);

	debug("Hop datapath_id : %d\n", h->datapath_id);
	debug("Path n_hops : %d\n", p->n_hops);
	debug("end\n");

	return Qnil;
}

static VALUE flow_manager_append_hops_to_path(VALUE self, VALUE flow_manager_path, VALUE rhops)
{
	debug("start\n");
	path *p;
	Data_Get_Struct(flow_manager_path, path, p);

	  if ( rhops != Qnil ) {
	    switch ( TYPE( rhops ) ) {
	      case T_ARRAY:
	        {
	          VALUE *each = RARRAY_PTR( rhops );
	          int i;
	          for ( i = 0; i < RARRAY_LEN( rhops ); i++ ) {
	        		append_hop_to_path(p, each[i]);
	          }
	        }
	        break;
	      default:
	        rb_raise( rb_eTypeError, "hops argument must be an Array" );
	    }
	  }

	debug("end_\n");
	return Qnil;
}

static VALUE init_flow_manager(VALUE self)
{
	debug("start\n");
	init_path();
	debug("end\n");

	return Qnil;
}

void Init_flow_manager_module()
{
	mFlowManager = rb_define_module("Flow_manager");
	rb_define_module_function(mFlowManager, "initialize", init_flow_manager, 0);
	rb_define_module_function(mFlowManager, "append_hop_to_path", flow_manager_append_hop_to_path, 2);
	rb_define_module_function(mFlowManager, "append_hops_to_path", flow_manager_append_hops_to_path, 2);
	rb_define_module_function(mFlowManager, "setup", flow_manager_setup, 2);
	rb_define_module_function(mFlowManager, "lookup", flow_manager_lookup, 3);
	rb_define_module_function(mFlowManager, "teardown", flow_manager_teardown, 2);
}


