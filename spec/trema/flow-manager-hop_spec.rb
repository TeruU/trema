#
# Copyright (C) 2008-2011 NEC Corporation
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


require File.join( File.dirname( __FILE__ ), "..", "spec_helper" )
require "trema"

describe Hop, "test" do
  before do
    datapath_id = 0x1;
    in_port = 1;
    out_port = 2;
    @hop = Hop.new(datapath_id, in_port, out_port)

    datapath_id2 = 0x2;
    in_port2 = 2;
    out_port2 = 1;
    Array actions2 = Array.new([SendOutPort.new(:port_number => 1, :max_len => 256), 
                      SetEthSrcAddr.new("11:22:33:44:55:66"),
                      SetEthDstAddr.new("11:22:33:44:55:66"),
                      SetIpSrcAddr.new("192.168.1.1"),
                      SetIpDstAddr.new("192.168.1.1"),
                      SetIpTos.new(32),
                      SetTransportSrcPort.new( 5555 ),
                      SetTransportDstPort.new( 5555 ),
                      ActionSetVlanVid.new( 1 ),
                      SetVlanPriority.new( 7 ),
                      StripVlanHeader.new,
                      VendorAction.new( 0x00004cff, Array["test", "test2"] )])
    @hop2 = Hop.new(datapath_id2, in_port2, out_port2, actions2 )

    datapath_id3 = 0x3;
    in_port3 = 2;
    out_port3 = 1;
    action = VendorAction.new( 0x00004cff)
    @hop3 = Hop.new(datapath_id3, in_port3, out_port3, action)
  end

  it "no argument" do
    expect {Hop.new()}.to raise_error()
  end 

  it "3 arguments" do
    tempHop = Hop.new(0x1, 1, 2)
    tempHop.instance_of?(Hop).should == true
  end

  it "4 arugments with single actions" do
      action = SendOutPort.new(:port_number => 1, :max_len => 256)
      tempHop = Hop.new(0x1, 1, 2, action)
      tempHop.instance_of?(Hop).should == true
  end

  it "4 arugments with multiple actions" do
      Array actions = Array.new([SendOutPort.new(:port_number => 1, :max_len => 256), 
                SetEthSrcAddr.new("11:22:33:44:55:66"),
                SetEthDstAddr.new("11:22:33:44:55:66"),
                SetIpSrcAddr.new("192.168.1.1"),
                SetIpDstAddr.new("192.168.1.1"),
                SetIpTos.new(32),
                SetTransportSrcPort.new( 5555 ),
                SetTransportDstPort.new( 5555 ),
                ActionSetVlanVid.new( 1 ),
                SetVlanPriority.new( 7 ),
                StripVlanHeader.new,
                VendorAction.new( 0x00004cff, Array["test", "test2"] )])
      tempHop = Hop.new(0x1, 1, 2, actions)
      tempHop.instance_of?(Hop).should == true
  end

  it "4 arguments with strange action" do
      action = Match.new()
      expect {Hop.new(0x1, 1, 2, action)}.to raise_error() 
  end

  it "4 arguments with a bad action array" do
        datapath_id = 0x4;
        in_port = 2;
        out_port = 1;
        actions = Array.new([SendOutPort.new(:port_number => 1, :max_len => 256),
                              VendorAction.new( 0x00004cff),
                             Match.new() ] )
        expect {Hop.new(datapath_id, in_port, out_port, actions)}.to raise_error("actions argument must be an Array of Action objects")
  end

  it "4 arguments with a differenct object" do
        datapath_id = 0x4;
        in_port = 2;
        out_port = 1;
        actions = Match.new()
        expect {Hop.new(datapath_id, in_port, out_port, actions)}.to raise_error("actions argument must be an Array or an Action object")
  end

  it ".datapath_id" do
    	@hop.datapath_id.should == 0x1
      @hop2.datapath_id.should == 0x2
  end
  
  it ".in_port" do
    	@hop.in_port.should == 1
      @hop2.in_port.should == 2
  end
  
  it ".out_port" do
    	@hop.out_port.should == 2
      @hop2.out_port.should == 1
  end
  
  it ".actions" do
      	Array actions = @hop.actions
        p actions
        actions.should be_false

        Array actions2 = @hop2.actions
        p actions2
        actions2[0].instance_of?(SendOutPort).should == true
        actions2[0].port_number.should == 1
        actions2[0].max_len.should == 256
        actions2[1].instance_of?(SetEthSrcAddr).should == true
        actions2[1].mac_address.to_s.should == "11:22:33:44:55:66"
        actions2[2].instance_of?(SetEthDstAddr).should == true
        actions2[2].mac_address.to_s.should == "11:22:33:44:55:66"
        actions2[3].instance_of?(SetIpSrcAddr).should == true
        actions2[3].ip_address.to_s.should == "192.168.1.1"
        actions2[4].instance_of?(SetIpDstAddr).should == true
        actions2[4].ip_address.to_s.should == "192.168.1.1"
        actions2[5].instance_of?(SetIpTos).should == true
        actions2[5].type_of_service.should == 32
        actions2[6].instance_of?(SetTransportSrcPort).should == true
        actions2[6].port_number.should == 5555
        actions2[7].instance_of?(SetTransportDstPort).should == true
        actions2[7].port_number.should == 5555
        actions2[8].instance_of?(ActionSetVlanVid).should == true
        actions2[8].vlan_id.should == 1
        actions2[9].instance_of?(SetVlanPriority).should == true
        actions2[9].vlan_priority.should == 7
        actions2[10].instance_of?(StripVlanHeader).should == true
        actions2[11].instance_of?(VendorAction).should == true
        actions2[11].vendor_id.should == 0x00004cff
        Array body = actions2[11].body
        body[0].should == "test"
        body[1].should == "test2"

        action = @hop3.actions
        action.body.should be_nil 
  end
	
  after do
  	@hop = nil
    @hop2 = nil
    @hop3 = nil
  end
end

### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
