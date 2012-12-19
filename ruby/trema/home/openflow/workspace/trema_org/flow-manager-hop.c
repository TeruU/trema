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


VALUE cHop;

static VALUE hop_datapath_id(VALUE self)
{
	hop *h;
	Data_Get_Struct(self, hop, h);
	return INT2FIX(h->datapath_id);
}

static VALUE hop_in_port(VALUE self)
{
	hop *h;
	Data_Get_Struct(self, hop, h);
	return INT2FIX(h->in_port);
}

static VALUE hop_out_port(VALUE self)
{
	hop *h;
	Data_Get_Struct(self, hop, h);
	return INT2FIX(h->out_port);
}

static VALUE hop_actions(VALUE self)
{
	hop *h;
	Data_Get_Struct(self, hop, h);
	return h->extra_actions;
}


static VALUE create_Hop(VALUE klass)
{
	puts("start_create_hop");
	hop *h = xmalloc( sizeof( hop ) );
	memset( h, 0, sizeof( hop ) );
	return Data_Wrap_Struct(klass, 0, delete_hop, h );
}

//VALUE self shoud be the first place
//static VALUE hop_initialize(VALUE self, VALUE datapath_id, VALUE in_port, VALUE out_port, VALUE *actions)
static VALUE hop_initialize(int argc, VALUE *argv, VALUE self)
{
	hop *h;
	uint16_t _datapath_id;
	uint16_t _in_port;
	uint16_t _out_port;
	openflow_actions *_actions;

	VALUE datapath_id;
	VALUE in_port;
	VALUE out_port;
	VALUE *actions = NULL;

	int nargs = rb_scan_args(argc, argv, "31", &datapath_id, &in_port, &out_port, actions);
	Data_Get_Struct(self, hop, h);
	memset( h, 0, sizeof ( hop ) );

	switch(nargs)
	{
		case 3:
		{

			_datapath_id = NUM2UINT(datapath_id);
			_in_port = NUM2UINT(in_port);
			_out_port = NUM2UINT(out_port);
			_actions = NULL;

			h->datapath_id = _datapath_id;
			h->in_port = _in_port;
			h->out_port = _out_port;
			h->extra_actions = NULL;

			break;
		}
		case 4:
		{

			_datapath_id = NUM2UINT(datapath_id);
			_in_port = NUM2UINT(in_port);
			_out_port = NUM2UINT(out_port);
			_actions = NULL;

			h->datapath_id = _datapath_id;
			h->in_port = _in_port;
			h->out_port = _out_port;
			h->extra_actions = NULL;

			if (actions != NULL ) {
				h->extra_actions = create_actions();
				_actions = (openflow_actions *)actions;
				list_element *element = _actions->list;
				while ( element != NULL ) {
				  struct ofp_action_header *ah = element->data;
				  void *action = xmalloc( ah->len );
				  memcpy( action, ah, ah->len );
				  append_to_tail( &h->extra_actions->list, action );
				  h->extra_actions->n_actions++;
				  element = element->next;
				}
			}

			break;
		}
		default:
		{
		}
	}

	return Qnil;
}

void Init_hop()
{
	cHop = rb_define_class("Hop", rb_cObject);
	rb_define_alloc_func(cHop, create_Hop);
	//rb_define_private_method(cHop, "initialize", hop_initialize, 3);
	rb_define_private_method(cHop, "initialize", hop_initialize, -1);
	rb_define_method(cHop, "datapath_id", hop_datapath_id, 0);
	rb_define_method(cHop, "in_port", hop_in_port, 0);
	rb_define_method(cHop, "out_port", hop_out_port, 0);
	rb_define_method(cHop, "actions", hop_actions, 0);
}


