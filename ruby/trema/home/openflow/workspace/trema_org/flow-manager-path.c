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
 *
 *************************************************************************
 * typedef struct {
 * struct ofp_match match;
 * uint16_t priority;
 * uint16_t idle_timeout;
 * uint16_t hard_timeout;
 * int n_hops;
 * list_element *hops;
}* path;
 */

#include <string.h>
#include "trema.h"
#include "openflow.h"
#include "ruby.h"
#include "libpath.h"
#include "libpath.c"

VALUE cPath;

static VALUE path_priority(VALUE self)
{
	path *p;
	Data_Get_Struct(self, path, p);
	return INT2FIX(p->priority);
}

static VALUE path_idle_timeout(VALUE self)
{
	path *p;
	Data_Get_Struct(self, path, p);
	return INT2FIX(p->idle_timeout);
}

static VALUE path_hard_timeout(VALUE self)
{
	path *p;
	Data_Get_Struct(self, path, p);
	return INT2FIX(p->hard_timeout);
}


static VALUE create_Path(VALUE klass)
{
	puts("start_path_create_Path");
	path_private *pp = xmalloc( sizeof( path_private ) );
	memset(pp, 0, sizeof( path_private ) );
	pp->public.priority = 65535;
	pp->public.idle_timeout = 30;
	pp->public.hard_timeout = 30;
	pp->public.n_hops = 0;
	create_list( &pp->public.hops );

	puts("end_path_create_Path");
	path *p = &pp->public;
	return Data_Wrap_Struct(klass, 0, delete_path, p);
}

//VALUE self should be the first place
//static VALUE path_initialize(VALUE self, VALUE *match, VALUE priority, VALUE idle_timeout, VALUE hard_timeout)
static VALUE path_initialize(int argc, VALUE *argv, VALUE self)
{
	puts("start_path_initialize");
	path *p;
	Data_Get_Struct(self, path, p);

    //p->priority = 65535;
    //p->idle_timeout = 30;
    //p->hard_timeout = 30;
    //p->n_hops = 0;
    //create_list( &p->hops );

    //temp data
	struct ofp_match *_match;
	uint16_t _priority = 65535;
	uint16_t _idle_timeout = 30;
	uint16_t _hard_timeout = 30;

	VALUE match;
	VALUE options;

	int nargs = rb_scan_args(argc, argv, "11", &match, &options);
	printf("The number of args is %d\n", nargs);

	switch(nargs)
	{
		case 1:
		{
			puts("case1 passed");

		     Data_Get_Struct(match, struct ofp_match, _match );
			p->match = (struct ofp_match)*_match;
			printf("match pointer : %p\n", p->match);
			break;
		}
		case 2:
		{
			puts("case2 passed");
			if(options != Qnil)
			{
				//_match = (struct ofp_match)match;

				VALUE priority = rb_hash_aref(options, ID2SYM( rb_intern( "priority" ) ) );
				if ( priority != Qnil ) {
					_priority = ( uint16_t ) NUM2UINT( priority );
				}

				VALUE idle_timeout = rb_hash_aref(options, ID2SYM( rb_intern( "idle_timeout" ) ) );
				if ( idle_timeout != Qnil ) {
					_idle_timeout = ( uint16_t ) NUM2UINT( idle_timeout );
				}

				VALUE hard_timeout = rb_hash_aref(options, ID2SYM( rb_intern( "hard_timeout" ) ) );
				if ( hard_timeout != Qnil ) {
					_hard_timeout = ( uint16_t ) NUM2UINT( hard_timeout );
				}
			}

			break;
		}
		default:
		{
		}
	}

	puts("end_path_initialize");
	return Qnil;
}

void Init_path()
{
	cPath = rb_define_class("Path", rb_cObject);
	rb_define_alloc_func(cPath, create_Path);
	rb_define_private_method(cPath, "initialize", path_initialize, -1);
	//rb_define_method(cPath, "match", path_match, 0);
	rb_define_method(cPath, "priority", path_priority, 0);
	rb_define_method(cPath, "idle_timeout", path_idle_timeout, 0);
	rb_define_method(cPath, "hard_timeout", path_hard_timeout, 0);
}


