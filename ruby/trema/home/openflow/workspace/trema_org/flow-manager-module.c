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


VALUE mFlowManager;
static VALUE flow_manager_setup(VALUE self, VALUE flow_manager_path)
{
	puts("start_flow_manager_setup");
	path *p;
	Data_Get_Struct(flow_manager_path, path, p);

	printf("path hops : %d\n", p->n_hops);

	bool ret = setup_path( p, handle_setup, NULL, handle_teardown, NULL );

	printf("The result of setup is %d", ret);
	puts("end_flow_manager_setup");
}

static VALUE flow_manager_append_hop_to_path(VALUE self, VALUE flow_manager_path, VALUE flow_manager_hop)
{
	puts("start_flow_manager_module_append_hop_to_path");
	path *p;
	hop *h;
	Data_Get_Struct(flow_manager_path, path, p);
	Data_Get_Struct(flow_manager_hop, hop, h);

	append_hop_to_path(p, h);

	printf("Hop datapath_id : %d\n", h->datapath_id);
	printf("Path n_hops : %d\n", p->n_hops);
	puts("end_flow_manager_module_append_hop_to_path");

	return Qnil;
}

static VALUE init_flow_manager(VALUE self)
{
	puts("start_init_flow_manager");
	init_path();
	puts("end_init_flow_manager");

	return Qnil;
}

void Init_flow_manager_module()
{
	mFlowManager = rb_define_module("Flow_manager");
	rb_define_module_function(mFlowManager, "initialize", init_flow_manager, 0);
	rb_define_module_function(mFlowManager, "append_hop_to_path", flow_manager_append_hop_to_path, 2);
	rb_define_module_function(mFlowManager, "setup", flow_manager_setup, 1);
}


