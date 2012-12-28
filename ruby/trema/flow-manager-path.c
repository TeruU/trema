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
 */

#include <string.h>
#include <memory.h>
#include "trema.h"
#include "openflow.h"
#include "ruby.h"
#include "libpath.h"
#include "match.h"
#include "flow-manager-hop.h"
#include "utils.h"

//#define DEBUG
#ifdef DEBUG
#define debug(...) {printf("%s(%d):", __func__, __LINE__); printf(__VA_ARGS__);}
#else
#define debug(...) 1 ? (void) 0 : printf
#endif

//This structure exists in libpath.c
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

VALUE cPath;

static void
dump_match( const struct ofp_match *match ) {
  char match_string[ 256 ];
  match_to_string( match, match_string, sizeof( match_string ) );
  info( "Match: match = [%s]",match_string);
}

static VALUE path_priority(VALUE self)
{
	path *p;
	Data_Get_Struct(self, path, p);
	return UINT2NUM(p->priority);
}

static VALUE path_idle_timeout(VALUE self)
{
	path *p;
	Data_Get_Struct(self, path, p);
	return UINT2NUM(p->idle_timeout);
}

static VALUE path_hard_timeout(VALUE self)
{
	path *p;
	Data_Get_Struct(self, path, p);
	return UINT2NUM(p->hard_timeout);
}

static VALUE path_hops(VALUE self)
{
    debug("start\n");

    VALUE hops = rb_ary_new();

    path *p;
    Data_Get_Struct(self, path, p);

    list_element *element = p->hops;
    while ( element != NULL ) {
            hop_private *temp = (hop_private *)element->data;

            //VALUE rHop = Data_Wrap_Struct(cHop, 0, -1, &temp->public );
            //rb_ary_push(hops, rHop);
            rb_ary_push(hops, (VALUE)temp->r_hop_pointer);

            element = element->next;
    }

    debug("end\n");
    return hops;
}

static VALUE path_match(VALUE self)
{
	debug("start\n");
	path *p;
	Data_Get_Struct(self, path, p);
	dump_match(&p->match);

	VALUE obj;
	obj = rb_funcall( cMatch, rb_intern( "new" ), 0 );
	struct ofp_match *match;
	Data_Get_Struct( obj, struct ofp_match, match );

	memcpy(match->dl_dst, p->match.dl_dst, OFP_ETH_ALEN);
	memcpy(match->dl_src, p->match.dl_src, OFP_ETH_ALEN);
	match->dl_type = p->match.dl_type;
	match->dl_vlan = p->match.dl_vlan;
	match->dl_vlan_pcp = p->match.dl_vlan_pcp;
	match->in_port = p->match.in_port;
	match->nw_dst = p->match.nw_dst;
	match->nw_proto = p->match.nw_proto;
	match->nw_src = p->match.nw_src;
	match->nw_tos = p->match.nw_tos;
	match->tp_dst = p->match.tp_dst;
	match->tp_src = p->match.tp_src;
	match->wildcards = p->match.wildcards;

	debug("end\n");
	return obj;
}

static VALUE path_initialize(int argc, VALUE *argv, VALUE self)
{
	debug("start\n");
	path *p;
	Data_Get_Struct(self, path, p);

	struct ofp_match *_match;
	VALUE options;
	VALUE match;

	int nargs = rb_scan_args(argc, argv, "11", &match, &options);
        Data_Get_Struct(match, struct ofp_match, _match );
	p->match = (struct ofp_match)*_match;
	dump_match(_match);

	switch(nargs)
	{
		case 1:
		{
			//Do nothing
			break;
		}
		case 2:
		{
			if(TYPE( options ) != T_HASH)
                        {
	                        rb_raise( rb_eTypeError, "The second argument should be hash." );
                        }

			if(options != Qnil)
			{
				VALUE priority = rb_hash_aref(options, ID2SYM( rb_intern( "priority" ) ) );
				if ( priority != Qnil ) {
					p->priority = NUM2UINT( priority );
				}

				VALUE idle_timeout = rb_hash_aref(options, ID2SYM( rb_intern( "idle_timeout" ) ) );
				if ( idle_timeout != Qnil ) {
					p->idle_timeout = NUM2UINT( idle_timeout );
				}

				VALUE hard_timeout = rb_hash_aref(options, ID2SYM( rb_intern( "hard_timeout" ) ) );
				if ( hard_timeout != Qnil ) {
					p->hard_timeout = NUM2UINT( hard_timeout );
				}
			}
			break;
		}
		default:
		{
			rb_raise( rb_eTypeError, "The number of argument is invalid." );
			break;
		}
	}

	debug("end\n");
	return Qnil;
}

static void delete_Path(path* path)
{
    debug("start\n");
    path_private *private = ( path_private * ) path;
    printf("path %p will be deleted\n", private );
    delete_list( path->hops );
    xfree( private );
    debug("end\n");
}

static VALUE create_Path(VALUE klass)
{
	debug("start\n");
	path_private *pp = ALLOC( path_private );
	debug("pass private %p is created.\n", pp);
	pp->public.priority = 65535;
	pp->public.idle_timeout = 30;
	pp->public.hard_timeout = 30;
	pp->public.n_hops = 0;
	create_list( &pp->public.hops );

	path *p = &pp->public;
	debug("pass %p.\n", p);
	VALUE rPath = Data_Wrap_Struct(klass, 0, -1, p);
	debug("ruby pass %p is created.\n", rPath);
	debug("end\n");

	return rPath;
}

void Init_path()
{
	cPath = rb_define_class("Path", rb_cObject);
	rb_define_alloc_func(cPath, create_Path);
	rb_define_private_method(cPath, "initialize", path_initialize, -1);
	rb_define_method(cPath, "priority", path_priority, 0);
	rb_define_method(cPath, "idle_timeout", path_idle_timeout, 0);
	rb_define_method(cPath, "hard_timeout", path_hard_timeout, 0);
	rb_define_method(cPath, "match", path_match, 0);
	rb_define_method(cPath, "hops", path_hops, 0);
}
