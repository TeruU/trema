#
# Copyright (C) 2008-2012 NEC Corporation
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2, as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#


class FlowManagerController < Controller
  oneshot_timer_event(:test, 5)
  
  def start
	info "*************************************************************************
*********************Start FlowManagerController*************************
*************************************************************************"
  end
  
  def flow_manager_setup_reply(status, path)
  	info "************************flow_manager_setup_reply********************" 
  	info status  	
    p path
    p path.priority()
    p path.idle_timeout()
    p path.hard_timeout()
    p path.match()
    arrHops = path.hops()
    p arrHops[0].datapath_id()
    p arrHops[0].in_port()
    p arrHops[0].out_port()
    p arrHops[0].actions()
    p arrHops[1].datapath_id()
    p arrHops[1].in_port()
    p arrHops[1].out_port()
    p arrHops[1].actions()

    match = Match.new(:in_port => 1)
    p match
    puts "****************lookup start******************"
    
    lookuppath = Flow_manager.lookup(0x1, match, 65535)
    info "path lookup success"
    p lookuppath
    p lookuppath.priority()
    p lookuppath.idle_timeout()
    p lookuppath.hard_timeout()
    p lookuppath.match()
    puts "*****************lookup finish***************"
  end
  
  def flow_manager_teardown_reply(reason, path)
  	info "*************************flow_manager_teardown_reply*****************" 
  	info reason
    p path
    p path.priority()
    p path.idle_timeout()
    p path.hard_timeout()
    p path.match()
    arrHops = path.hops()
    p arrHops[0].datapath_id()
    p arrHops[0].in_port()
    p arrHops[0].out_port()
    p arrHops[0].actions()
    p arrHops[1].datapath_id()
    p arrHops[1].in_port()
    p arrHops[1].out_port()
    p arrHops[1].actions()
  end 
  
  def switch_ready datapath_id
 	info "***Hello %#x from #{ ARGV[ 0 ] }!" % datapath_id
  end
  
  def test
    Flow_manager.initialize()
  
    Array actions = [StripVlanHeader.new, SendOutPort.new(1)]

  	hop = Hop.new(0x1, 1, 2, actions)
	  p hop
	  p hop.datapath_id()
	  p hop.in_port()
	  p hop.out_port()
    p hop.actions()

    hop2 = Hop.new(0x2, 2, 1)
	
  	match = Match.new(:in_port => 1)
  	p match
  	
    path = Path.new(match, options={:idle_timeout=>5})
    p path
    p path.priority()
    p path.idle_timeout()
    p path.hard_timeout()
    p path.match()
    
    Flow_manager.append_hop_to_path(path, hop)
    Flow_manager.append_hop_to_path(path, hop2)
    
    Flow_manager.setup(path,self)

    info "*******************exit switch ready FlowManagerController*****************"
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8
### indent-tabs-mode: nil
### End:
