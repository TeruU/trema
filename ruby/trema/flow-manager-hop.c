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
#include "trema.h"
#include "ruby.h"
#include "libpath.h"
#include "flow-manager-hop.h"

#define DEBUG
#ifdef DEBUG
#define debug(...) {printf("%s(%d):", __func__, __LINE__); printf(__VA_ARGS__);}
#else
#define debug(...) 1 ? (void) 0 : printf
#endif

VALUE cHop;

static void form_actions( VALUE raction, openflow_actions *actions );
static void flow_manager_append_action( openflow_actions *actions, VALUE action );

static void form_actions( VALUE raction, openflow_actions *actions )
{
  debug("start\n");
  if ( raction != Qnil ) {
    switch ( TYPE( raction ) ) {
      case T_ARRAY:
        {
          debug("actions Type : T_ARRAY\n");
          VALUE *each = RARRAY_PTR( raction );
          int i;
          debug("RARRAY_LEN : %d\n", RARRAY_LEN( raction));
          for ( i = 0; i < RARRAY_LEN( raction ); i++ ) {
        	  debug("Pointer of actions : %p\n", each[i]);
        	  flow_manager_append_action( actions, each[ i ] );
          }
          debug("check actions array (number) : %d\n", actions->n_actions);
        }
        break;
      case T_OBJECT:
    	  debug("actions Type : T_OBJECT\n");
    	  flow_manager_append_action( actions, raction );
        break;
      default:
    	  debug("actions Type : default\n");
          rb_raise( rb_eTypeError, "actions argument must be an Array or an Action object" );
    }
  }
  debug("end\n");
}

static void flow_manager_append_action( openflow_actions *actions, VALUE action ) {
  if ( rb_funcall( action, rb_intern( "is_a?" ), 1, rb_path2class( "Trema::Enqueue" ) ) == Qtrue ) {
    uint32_t queue_id = ( uint32_t ) NUM2UINT( rb_funcall( action, rb_intern( "queue_id" ), 0 ) );
    uint16_t port_number = ( uint16_t ) NUM2UINT( rb_funcall( action, rb_intern( "port_number" ), 0 ) );
    append_action_enqueue( actions, port_number, queue_id );
  }
  else if ( rb_funcall( action, rb_intern( "is_a?" ), 1, rb_path2class( "Trema::SendOutPort" ) ) == Qtrue ) {
    uint16_t port_number = ( uint16_t ) NUM2UINT( rb_funcall( action, rb_intern( "port_number" ), 0 ) );
    uint16_t max_len = ( uint16_t ) NUM2UINT( rb_funcall( action, rb_intern( "max_len" ), 0 ) );
    append_action_output( actions, port_number, max_len );
  }
  else if ( rb_funcall( action, rb_intern( "is_a?" ), 1, rb_path2class( "Trema::SetEthDstAddr" ) ) == Qtrue ) {
    uint8_t dl_dst[ OFP_ETH_ALEN ];
    uint8_t *ptr = ( uint8_t* ) dl_addr_to_a( rb_funcall( action, rb_intern( "mac_address" ), 0 ), dl_dst );
    append_action_set_dl_dst( actions, ptr );
  }
  else if ( rb_funcall( action, rb_intern( "is_a?" ), 1, rb_path2class( "Trema::SetEthSrcAddr" ) ) == Qtrue ) {
    uint8_t dl_src[ OFP_ETH_ALEN ];
    uint8_t *ptr = ( uint8_t* ) dl_addr_to_a( rb_funcall( action, rb_intern( "mac_address" ), 0 ), dl_src );
    append_action_set_dl_src( actions, ptr );
  }
  else if ( rb_funcall( action, rb_intern( "is_a?" ), 1, rb_path2class( "Trema::SetIpDstAddr" ) ) == Qtrue ) {
    append_action_set_nw_dst( actions, nw_addr_to_i( rb_funcall( action, rb_intern( "ip_address" ), 0 ) ) );
  }
  else if ( rb_funcall( action, rb_intern( "is_a?" ), 1, rb_path2class( "Trema::SetIpSrcAddr" ) ) == Qtrue ) {
    append_action_set_nw_src( actions, nw_addr_to_i( rb_funcall( action, rb_intern( "ip_address" ), 0 ) ) );
  }
  else if ( rb_funcall( action, rb_intern( "is_a?" ), 1, rb_path2class( "Trema::SetIpTos" ) ) == Qtrue ) {
    append_action_set_nw_tos( actions, ( uint8_t ) NUM2UINT( rb_funcall( action, rb_intern( "type_of_service" ), 0 ) ) );
  }
  else if ( rb_funcall( action, rb_intern( "is_a?" ), 1, rb_path2class( "Trema::SetTransportDstPort" ) ) == Qtrue ) {
    append_action_set_tp_dst( actions, ( uint16_t ) NUM2UINT( rb_funcall( action, rb_intern( "port_number" ), 0 ) ) );
  }
  else if ( rb_funcall( action, rb_intern( "is_a?" ), 1, rb_path2class( "Trema::SetTransportSrcPort" ) ) == Qtrue ) {
    append_action_set_tp_src( actions, ( uint16_t ) NUM2UINT( rb_funcall( action, rb_intern( "port_number" ), 0 ) ) );
  }
  else if ( rb_funcall( action, rb_intern( "is_a?" ), 1, rb_path2class( "Trema::SetVlanPriority" ) ) == Qtrue ) {
    append_action_set_vlan_pcp( actions, ( uint8_t ) NUM2UINT( rb_funcall( action, rb_intern( "vlan_priority" ), 0 ) ) );
  }
  else if ( rb_funcall( action, rb_intern( "is_a?" ), 1, rb_path2class( "Trema::SetVlanVid" ) ) == Qtrue ) {
    append_action_set_vlan_vid( actions, ( uint16_t ) NUM2UINT( rb_funcall( action, rb_intern( "vlan_id" ), 0 ) ) );
  }
  else if ( rb_funcall( action, rb_intern( "is_a?" ), 1, rb_path2class( "Trema::StripVlanHeader" ) ) == Qtrue ) {
    append_action_strip_vlan( actions );
  }
  else if ( rb_funcall( action, rb_intern( "is_a?" ), 1, rb_path2class( "Trema::VendorAction" ) ) == Qtrue ) {
    VALUE vendor_id = rb_funcall( action, rb_intern( "vendor_id" ), 0 );
    VALUE rbody = rb_funcall( action, rb_intern( "body" ), 0 );
    if ( rbody != Qnil ) {
      Check_Type( rbody, T_ARRAY );
      uint16_t length = ( uint16_t ) RARRAY_LEN( rbody );
      buffer *body = alloc_buffer_with_length( length );
      int i;
      for ( i = 0; i < length; i++ ) {
        ( ( uint8_t * ) body->data )[ i ] = ( uint8_t ) FIX2INT( RARRAY_PTR( rbody )[ i ] );
      }
      append_action_vendor( actions, ( uint32_t ) NUM2UINT( vendor_id ), body );
      free_buffer( body );
    }
    else {
      append_action_vendor( actions, ( uint32_t ) NUM2UINT( vendor_id ), NULL );
    }
  }
  else {
    rb_raise( rb_eTypeError, "actions argument must be an Array of Action objects" );
  }
}

static VALUE make_actions_array(openflow_actions *actions)
{
	VALUE actionsArray = rb_ary_new();

    if ( actions != NULL ) {
      list_element *element = actions->list;
	  while ( element != NULL ) {
	    struct ofp_action_header *ah = element->data;
	    debug("action len : %d\n", ah->len );
	    debug("action type : %d\n", ah->type);

	    if(ah->type == OFPAT_OUTPUT)
	    {
		    VALUE actionInfoHash = rb_hash_new();
	    	struct ofp_action_output *action = element->data;

	        rb_hash_aset(actionInfoHash, rb_str_new2("type"), INT2NUM(action->type));
	        rb_hash_aset(actionInfoHash, rb_str_new2("port_number"), INT2NUM(action->port));
	        rb_hash_aset(actionInfoHash, rb_str_new2("max_len"), INT2NUM(action->max_len));
	        rb_ary_push(actionsArray, actionInfoHash);
	    }
	    else if(ah->type == OFPAT_SET_VLAN_VID)
	    {
	    	VALUE actionInfoHash = rb_hash_new();
	    	struct ofp_action_vlan_vid *action = element->data;

	        rb_hash_aset(actionInfoHash, rb_str_new2("type"), INT2NUM(action->type));
	        rb_hash_aset(actionInfoHash, rb_str_new2("vlan_vid"), INT2NUM(action->vlan_vid) );
	        rb_ary_push(actionsArray, actionInfoHash);
	    }
	    else if (ah->type == OFPAT_SET_VLAN_PCP)
	    {
	    	VALUE actionInfoHash = rb_hash_new();
	    	struct ofp_action_vlan_pcp *action = element->data;

	        rb_hash_aset(actionInfoHash, rb_str_new2("type"), INT2NUM(action->type));
	        rb_hash_aset(actionInfoHash, rb_str_new2("vlan_pcp"), INT2NUM(action->vlan_pcp) );
	        rb_ary_push(actionsArray, actionInfoHash);
	    }
	    else if(ah->type == OFPAT_STRIP_VLAN)
	    {
		    VALUE actionInfoHash = rb_hash_new();
	    	struct ofp_action_header *action = element->data;

	        rb_hash_aset(actionInfoHash, rb_str_new2("type"), INT2NUM(action->type));
	        rb_ary_push(actionsArray, actionInfoHash);
	    }
	    else if(ah->type == OFPAT_SET_DL_SRC || ah->type == OFPAT_SET_DL_DST)
	    {
	    	VALUE actionInfoHash = rb_hash_new();
	    	struct ofp_action_dl_addr *action = element->data;

	        rb_hash_aset(actionInfoHash, rb_str_new2("type"), INT2NUM(action->type));
	        rb_hash_aset(actionInfoHash, rb_str_new2("dl_addr"), INT2NUM(action->dl_addr) );
	        rb_ary_push(actionsArray, actionInfoHash);

	    }
	    else if(ah->type == OFPAT_SET_NW_SRC || ah->type == OFPAT_SET_NW_DST)
	    {
	    	VALUE actionInfoHash = rb_hash_new();
	    	struct ofp_action_nw_addr *action = element->data;

	        rb_hash_aset(actionInfoHash, rb_str_new2("type"), INT2NUM(action->type));
	        rb_hash_aset(actionInfoHash, rb_str_new2("nw_addr"), INT2NUM(action->nw_addr) );
	        rb_ary_push(actionsArray, actionInfoHash);
	    }
	    else if(ah->type == OFPAT_SET_NW_TOS)
	    {
	    	VALUE actionInfoHash = rb_hash_new();
	    	struct ofp_action_nw_tos *action = element->data;

	        rb_hash_aset(actionInfoHash, rb_str_new2("type"), INT2NUM(action->type));
	        rb_hash_aset(actionInfoHash, rb_str_new2("nw_tos"), INT2NUM(action->nw_tos) );
	        rb_ary_push(actionsArray, actionInfoHash);
	    }
	    else if(ah->type == OFPAT_SET_TP_SRC || ah->type == OFPAT_SET_TP_DST)
	    {
	    	VALUE actionInfoHash = rb_hash_new();
	    	struct ofp_action_tp_port *action = element->data;

	        rb_hash_aset(actionInfoHash, rb_str_new2("type"), INT2NUM(action->type));
	        rb_hash_aset(actionInfoHash, rb_str_new2("tp_port"), INT2NUM(action->tp_port) );
	        rb_ary_push(actionsArray, actionInfoHash);
	    }
	    else if(ah->type == OFPAT_ENQUEUE)
	    {
	    	VALUE actionInfoHash = rb_hash_new();
	    	struct ofp_action_enqueue *action = element->data;

	        rb_hash_aset(actionInfoHash, rb_str_new2("type"), INT2NUM(action->type));
	        rb_hash_aset(actionInfoHash, rb_str_new2("port"), INT2NUM(action->port) );
	        rb_hash_aset(actionInfoHash, rb_str_new2("queue_id"), INT2NUM(action->queue_id) );
	        rb_ary_push(actionsArray, actionInfoHash);
	    }
	    else if(ah->type == OFPAT_VENDOR)
	    {
	    	VALUE actionInfoHash = rb_hash_new();
	    	struct ofp_action_vendor_header *action = element->data;

	        rb_hash_aset(actionInfoHash, rb_str_new2("type"), INT2NUM(action->type));
	        rb_hash_aset(actionInfoHash, rb_str_new2("vendor"), INT2NUM(action->vendor) );
	        rb_ary_push(actionsArray, actionInfoHash);
	    }

	    element = element->next;
	  }
    }
	return actionsArray;
}

static VALUE hop_datapath_id(VALUE self)
{
	hop *h;
	Data_Get_Struct(self, hop, h);
	return UINT2NUM(h->datapath_id);
}

static VALUE hop_in_port(VALUE self)
{
	hop *h;
	Data_Get_Struct(self, hop, h);
	return UINT2NUM(h->in_port);
}

static VALUE hop_out_port(VALUE self)
{
	hop *h;
	Data_Get_Struct(self, hop, h);
	return UINT2NUM(h->out_port);
}

static VALUE hop_actions(VALUE self)
{
	debug("start\n");

	hop_private *hp;
	Data_Get_Struct(self, hop_private, hp);

	debug("end\n");
	return (VALUE)hp->r_extra_actions_pointer;
}

/*
static VALUE hop_actions(VALUE self, VALUE actionObj)
{
	debug("start\n");
	hop *h;
	Data_Get_Struct(self, hop, h);

	VALUE actionsArray = make_actions_array(h->extra_actions);

	debug("end\n");
	return actionsArray;
}
*/

/*
static VALUE hop_actions(VALUE self, VALUE actionObj)
{
	debug("start\n");
	hop *h;
	Data_Get_Struct(self, hop, h);

	VALUE test = rb_funcall(rb_path2class( "Trema::SendOutPort" ), rb_intern( "new" ), 1, INT2NUM(1));

	int i = TYPE(test);
	debug("Type : %d\n", i);

	VALUE actionsArray = make_actions_array(h->extra_actions);

	debug("end\n");
	return actionsArray;
}
*/

/*
static VALUE hop_initialize(int argc, VALUE *argv, VALUE self)
{
	hop *h;
	Data_Get_Struct(self, hop, h);

	VALUE datapath_id;
	VALUE in_port;
	VALUE out_port;
	VALUE actions;
	int nargs = rb_scan_args(argc, argv, "31", &datapath_id, &in_port, &out_port, &actions);

	openflow_actions *_actions;

	switch(nargs)
	{
		case 3:
		{
			debug("case 3 passed\n");

			h->datapath_id = NUM2UINT(datapath_id);
			h->in_port = NUM2UINT(in_port);
			h->out_port = NUM2UINT(out_port);
			h->extra_actions = NULL;

			break;
		}
		case 4:
		{
			debug("case 4 passed\n");

			_actions = create_actions();

			h->datapath_id = NUM2UINT(datapath_id);
			h->in_port = NUM2UINT(in_port);
			h->out_port = NUM2UINT(out_port);
			h->extra_actions = NULL;

			form_actions(actions, _actions);
			if(_actions != NULL){
				h->extra_actions = _actions;
			}

			break;
		}
		default:
		{
			rb_raise( rb_eTypeError, "The number of argument is invalid." );
		}
	}

	return Qnil;
}

static VALUE create_Hop(VALUE klass)
{
	hop *h = xmalloc( sizeof( hop ) );
	memset( h, 0, sizeof( hop ) );
	return Data_Wrap_Struct(klass, 0, delete_hop, h );
}
*/

static VALUE hop_initialize(int argc, VALUE *argv, VALUE self)
{
	hop_private *hp;
	Data_Get_Struct(self, hop_private, hp);

	VALUE datapath_id;
	VALUE in_port;
	VALUE out_port;
	VALUE actions;
	int nargs = rb_scan_args(argc, argv, "31", &datapath_id, &in_port, &out_port, &actions);

	openflow_actions *_actions;

	switch(nargs)
	{
		case 3:
		{
			debug("case 3 passed\n");

			hp->public.datapath_id = NUM2UINT(datapath_id);
			hp->public.in_port = NUM2UINT(in_port);
			hp->public.out_port = NUM2UINT(out_port);
			hp->public.extra_actions = NULL;

			break;
		}
		case 4:
		{
			debug("case 4 passed\n");

			hp->public.datapath_id = NUM2UINT(datapath_id);
			hp->public.in_port = NUM2UINT(in_port);
			hp->public.out_port = NUM2UINT(out_port);
			hp->public.extra_actions = NULL;

			_actions = create_actions();
			form_actions(actions, _actions);
			if(_actions != NULL){
				hp->public.extra_actions = _actions;
			}

			hp->r_extra_actions_pointer = actions;

			break;
		}
		default:
		{
			rb_raise( rb_eTypeError, "The number of argument is invalid." );
		}
	}

	return Qnil;
}

static VALUE create_Hop(VALUE klass)
{
	hop_private *ph = xmalloc( sizeof( hop_private ) );
	memset( ph, 0, sizeof( hop_private ) );

	hop *h = &ph->public;
	ph->r_hop_pointer = Data_Wrap_Struct(klass, 0, delete_hop, h );

	return ph->r_hop_pointer;
}

/**
 * Hop Class
 * How to new:
 * 	Hop Hop.new(Integer datapath_id, Integer in_port, Integer out_port, Array actions)
 */
void Init_hop()
{
	cHop = rb_define_class("Hop", rb_cObject);
	rb_define_alloc_func(cHop, create_Hop);
	rb_define_private_method(cHop, "initialize", hop_initialize, -1);
	rb_define_method(cHop, "datapath_id", hop_datapath_id, 0);
	rb_define_method(cHop, "in_port", hop_in_port, 0);
	rb_define_method(cHop, "out_port", hop_out_port, 0);
	rb_define_method(cHop, "actions", hop_actions, 0);
}


