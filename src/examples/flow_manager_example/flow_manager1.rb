#
# Hello trema world!
#
# Author: Yasuhito Takamiya <yasuhito@gmail.com>
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

  def start
	info "*************************************************************************
*********************Start FlowManagerController*************************
*************************************************************************"
  end
  
  def flow_manager_setup_reply(status, path)
  	info "***flow_manager_setup_reply***" 
  	info status
  end
  
  def flow_manager_teardown_reply(reason, path)
  	info "***flow_manager_teardown_reply***" 
  	info reason
  end 
  
  def switch_ready datapath_id
    info "Hello %#x from #{ ARGV[ 0 ] }!" % datapath_id
   
    Flow_manager.initialize()
  	
  	#actions = []
  	#action0 = ActionOutput.new(:port => 1)
  	#actions[0] = action0
  	#action1 = ActionOutput.new(:port => 2)
  	#actions[1] = action1
  	#info "********************actions : "
  	#p actions
  
  	hop = Hop.new(0x1,1,2)
	p hop
	p hop.datapath_id()
	p hop.in_port()
	p hop.out_port()

  	
  	#actions2 = []
  	#action3 = ActionOutput.new(:port => 1)
  	#actions2[0] = action3
  	#action4 = ActionOutput.new(:port => 2)
  	#actions2[1] = action4
  	#info "********************actions2 : "
  	#p actions2
  	
  	hop2 = Hop.new(0x2,1,2)
  	p hop2
	p hop2.datapath_id()
	p hop2.in_port()
	p hop2.out_port()
  	
  	
  	#match = Match.new()
  	match = Match.new(:in_port => 1)
  	info "***************************match : " 
  	p match
  	
    path = Path.new(match)
    p path
    p path.priority()
    p path.idle_timeout()
    p path.hard_timeout()
    p path.match()
    
    match2 = Match.new(:in_port => 2)
    info "***************************match2 : "
    p match2
    
    path2 = Path.new(match2)
    p path2
    p path2.priority()
    p path2.idle_timeout()
    p path2.hard_timeout()
    p path2.match()
    
    Flow_manager.append_hop_to_path(path,hop);
    Flow_manager.setup(path,self);

	bool = Flow_manager.teardown(0x1, path)
	
    info "exit switch ready FlowManagerController"
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8
### indent-tabs-mode: nil
### End:
