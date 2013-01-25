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
#include "path.h"
#include "flow-manager-path.h"
#include "flow-manager-hop.h"
#include "match.h"
#include "path_utils.h"
#include "flow_manager_interface.h"

//#define DEBUG
#ifdef DEBUG
#define debug(...) {printf("%s(%d):", __func__, __LINE__); printf(__VA_ARGS__);}
#else
#define debug(...) 1 ? (void) 0 : printf
#endif

typedef struct {
  path public;
  uint64_t id;
  uint64_t in_datapath_id;
  setup_handler setup_callback;
  void *setup_user_data;
  teardown_handler teardown_callback;
  void *teardown_user_data;
} path_private;

typedef struct {
  hop public;
  void *r_hop_pointer;
  void *r_extra_actions_pointer;
} hop_private;

VALUE mFlowManager;

static void
handle_setup( int status, const path *p, void *controller ) {
  info( "**** Path setup completed ( status = %s)*** ", status_to_string( status ));

  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wcast-qual"
  VALUE obj = Data_Wrap_Struct(cPath, 0, 0, (void *)p);
  #pragma GCC diagnostic pop
  debug("path pointer : %p\n", p);
  debug(dump_path( p ));

  info("status:%s", status_to_string( status ));
  if(status != 0)
  {
	  info("flow-manager-module : 68");
	  rb_raise(rb_eException, "Error occured : %s", status_to_string( status ) );
  }

  if ( rb_respond_to( ( VALUE ) controller, rb_intern( "flow_manager_setup_reply" ) ) == Qtrue ) {
    rb_funcall( ( VALUE ) controller, rb_intern( "flow_manager_setup_reply" ), 2, rb_str_new2(status_to_string( status )), obj);
  }
}

static void
handle_teardown( int reason, const path *p, void *controller ) {
  info( "***Path teardown completed ( reason = %s)*** ", reason_to_string( reason ));

  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wcast-qual"
  VALUE obj = Data_Wrap_Struct(cPath, 0, 0, (void *)p);
  #pragma GCC diagnostic pop
  debug("path pointer : %p\n", p);
  debug(dump_path( p ));

  if ( rb_respond_to( ( VALUE ) controller, rb_intern( "flow_manager_teardown_reply" ) ) == Qtrue ) {
    rb_funcall( ( VALUE ) controller, rb_intern( "flow_manager_teardown_reply" ), 2, rb_str_new2(reason_to_string( reason )), obj);
  }
  //stop_trema();
}

static VALUE flow_manager_teardown_by_match(VALUE self, VALUE r_match)
{
    debug("start\n");
    UNUSED( self );

    struct ofp_match *m;
    Data_Get_Struct(r_match, struct ofp_match, m);

    bool ret = teardown_path_by_match(*m);
    debug("end with %d\n", ret);

    if(ret == 1)
    {
            return Qtrue;
    }
    else
    {
            return Qfalse;
    }
}

/*
static VALUE flow_manager_teardown(VALUE self, VALUE in_datapath_id ,VALUE flow_manager_path)
{
    debug("start\n");
    path *p;
    Data_Get_Struct(flow_manager_path, path, p);
    debug(dump_path( p ));

    uint64_t _in_datapath_id = NUM2UINT(in_datapath_id);
    struct ofp_match _match = p->match;
    uint16_t _priority = p->priority;

    bool ret = teardown_path(_in_datapath_id, _match, _priority );
    debug("end with %d\n", ret);

    if(ret == 1)
    {
            return Qtrue;
    }
    else
    {
            return Qfalse;
    }
}
*/

static VALUE flow_manager_teardown(VALUE self, VALUE in_datapath_id ,VALUE match, VALUE priority)
{
    debug("start\n");
    UNUSED( self );

    struct ofp_match *_match;
    Data_Get_Struct(match, struct ofp_match, _match);

    uint64_t _in_datapath_id = (uint64_t)NUM2UINT(in_datapath_id);
    uint16_t _priority = (uint16_t)NUM2UINT(priority);

    bool ret = teardown_path(_in_datapath_id, *_match, _priority );
    debug("end with %d\n", ret);

    if(ret == 1)
    {
            return Qtrue;
    }
    else
    {
            return Qfalse;
    }
}

static VALUE flow_manager_lookup(VALUE self, VALUE datapath_id, VALUE match, VALUE priority)
{
    debug("start\n");
    UNUSED( self );

    struct ofp_match *_match;
    uint64_t _datapath_id = (uint64_t)NUM2UINT(datapath_id);
    uint16_t _proirity = (uint16_t)NUM2UINT(priority);
    Data_Get_Struct(match, struct ofp_match, _match );

    const path *p = lookup_path(_datapath_id, *_match, _proirity);

    VALUE match2 = rb_funcall(cMatch, rb_intern("new"), 0);
    VALUE obj = rb_funcall( cPath, rb_intern( "new" ), 1, match2 );

    path *_path;
    Data_Get_Struct( obj, path, _path );

    debug(dump_path( p ));
    _path->hard_timeout = p->hard_timeout;
    _path->hops = p->hops;
    _path->idle_timeout = p->idle_timeout;
    _path->match = p->match;
    _path->n_hops = p->n_hops;
    _path->priority = p->priority;

    debug("end\n");
    return obj;
}

static VALUE flow_manager_setup(VALUE self, VALUE r_path, VALUE controller)
{
    debug("start\n");
    UNUSED( self );

    path *p;
    Data_Get_Struct(r_path, path, p);
    debug("path pointer : %p\n", p);

    bool ret = setup_path( p, handle_setup, (void *)controller, handle_teardown, (void *)controller );
    debug("end with %d\n", ret);

    if(ret == true)
    {
            return Qtrue;
    }
    else
    {
            return Qfalse;
    }
}

static VALUE flow_manager_append_hop_to_path(VALUE self, VALUE rpath, VALUE rhop)
{
    debug("start\n");
    UNUSED( self );

    path *p;
    hop *h;
    Data_Get_Struct(rpath, path, p);
    Data_Get_Struct(rhop, hop, h);

    append_hop_to_path(p, h);

    debug("end\n");

    return Qnil;
}

static VALUE flow_manager_append_hops_to_path(VALUE self, VALUE rpath, VALUE rhops)
{
    debug("start\n");
    UNUSED( self );

    path *p;
    //hop *h;

    Data_Get_Struct(rpath, path, p);

    if ( rhops != Qnil ) {
      debug("type : %d\n", TYPE(rhops));
      switch ( TYPE( rhops ) ) {
        case T_ARRAY:
          {
            VALUE *each = RARRAY_PTR( rhops );
            int i;
            for ( i = 0; i < RARRAY_LEN( rhops ); i++ ) {
                          hop *h;
                          Data_Get_Struct(each[i], hop, h);
                          append_hop_to_path(p, h);
            }
          }
          break;
        default:
          debug("start7\n");
          rb_raise( rb_eTypeError, "hops argument must be an Array" );
          break;
      }
    }

    debug("end_\n");
    return Qnil;
}




static VALUE init_flow_manager(VALUE self)
{
    debug("start\n");
    UNUSED( self );
    init_path();
    debug("end\n");

    return Qnil;
}

static VALUE finalize_flow_manager(VALUE self)
{
  debug("start\n");
  UNUSED( self );
  finalize_path();
  debug("end\n");

  return Qnil;
}

void Init_flow_manager_module()
{
    mFlowManager = rb_define_module("Flow_manager");
    rb_define_module_function(mFlowManager, "initialize", init_flow_manager, 0);
    rb_define_module_function(mFlowManager, "finalize", finalize_flow_manager, 0);
    rb_define_module_function(mFlowManager, "lookup", flow_manager_lookup, 3);
    rb_define_module_function(mFlowManager, "teardown_by_match", flow_manager_teardown_by_match, 1);
    rb_define_module_function(mFlowManager, "append_hop_to_path", flow_manager_append_hop_to_path, 2);
    rb_define_module_function(mFlowManager, "append_hops_to_path", flow_manager_append_hops_to_path, 2);
    rb_define_module_function(mFlowManager, "setup", flow_manager_setup, 2);
    rb_define_module_function(mFlowManager, "teardown", flow_manager_teardown, 3);
}


