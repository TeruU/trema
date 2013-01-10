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
	info "*************************************
*****Start FlowManagerController*****
*************************************"
  end
  
  def flow_manager_setup_reply(status, path)
  	info "*****flow_manager_setup_reply*****" 
  	info status
    info "path.priority:" + path.priority().inspect
    info "path.idle_timeout:" + path.idle_timeout().inspect
    info "path.hard_timeout:" + path.hard_timeout().inspect
    info "path.match:" + path.match().inspect
    info "path.hops:" + path.hops().inspect
  end
  
  def flow_manager_teardown_reply(reason, path)
  	info "*****flow_manager_teardown_reply*****" 
  	info reason
    info "path.priority:" + path.priority().inspect
    info "path.idle_timeout:" + path.idle_timeout().inspect
    info "path.hard_timeout:" + path.hard_timeout().inspect
    info "path.match:" + path.match().inspect
    info "path.hops:" + path.hops().inspect
  end 
  
  def switch_ready datapath_id
 	  info "***Hello %#x from #{ ARGV[ 0 ] }!" % datapath_id
  end
  
  def test
    Flow_manager.initialize()

    match = Match.new()

    hop = Hop.new(0x1,1,2)
    hop2 = Hop.new(0x2,2,1)
  
    path = Path.new(match, options={:priority=>65535, :idle_timeout=>5, :hard_timeout=>20})
    
    Flow_manager.append_hop_to_path(path, hop)
    Flow_manager.append_hop_to_path(path, hop2)

    info("***** Setting up a path *****")
    info "path.priority:" + path.priority().inspect
    info "path.idle_timeout:" + path.idle_timeout().inspect
    info "path.hard_timeout:" + path.hard_timeout().inspect
    info "path.match:" + path.match().inspect
    info "path.hops:" + path.hops().inspect

    Flow_manager.setup(path, self)

  end
end
