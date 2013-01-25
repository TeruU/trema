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
require 'trema/flow-manager'

class FlowManagerController < Controller
  include Trema::FlowManager
  oneshot_timer_event(:test, 5)
  
  def flow_manager_setup_reply(status, path)
  	info "************************flow_manager_setup_reply********************" 
  	info status  	
  end
  
  def flow_manager_teardown_reply(reason, path)
  	info "*************************flow_manager_teardown_reply*****************" 
  	info reason
  end 
  
  def switch_ready datapath_id
 	info "***Hello %#x from #{ ARGV[ 0 ] }!" % datapath_id
  end
  
  def test

    Array actions = [StripVlanHeader.new, SendOutPort.new(1)]
  
  	hop = Hop.new(0x1,1,2, actions)
	p hop
	p hop.datapath_id()
	p hop.in_port()
	p hop.out_port()
	p hop.actions()
	
	
	Array actions2 = [StripVlanHeader.new, SendOutPort.new(2)]
  
  	hop2 = Hop.new(0x2,2,3, actions2)
	p hop2
	p hop2.datapath_id()
	p hop2.in_port()
	p hop2.out_port()
	p hop2.actions()

    info "*******************exit switch ready FlowManagerController*****************"
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8
### indent-tabs-mode: nil
### End:
