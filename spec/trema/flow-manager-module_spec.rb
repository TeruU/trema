#
# Copyright (C) 2008-2013 NEC Corporation
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

describe Hop, ".new" do

  before do
    datapath_id = 0x1;
    in_port = 1;
    out_port = 2;
    @hop = Hop.new(datapath_id, in_port, out_port)

    datapath_id2 = 0x2;
    in_port2 = 2;
    out_port2 = 1;
    Array actions2 = [SendOutPort.new(:port_number => 1, :max_len => 256), 
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
                      VendorAction.new( 0x00004cff, Array["test", "test2"] ) ]
    @hop2 = Hop.new(datapath_id2, in_port2, out_port2, actions2 )

    match = Match.new()
    match2 = Match.new(
      :in_port => 1,
      :dl_src => "00:00:00:00:00:01",
      :dl_dst => "00:00:00:00:00:02",
      :dl_vlan => 65535,
      :dl_vlan_pcp => 0,
      :dl_type => 0x800,
      :nw_tos => 0,
      :nw_proto => 17,
      :nw_src => "192.168.0.1",
      :nw_dst => "192.168.0.0/24",
      :tp_src => 10,
      :tp_dst => 20
    )

    @path = Path.new(match, options={:idle_timeout=>5, :hard_timeout=>5, :priority=>65535})
    @path2 = Path.new(match2, options={:idle_timeout=>5, :hard_timeout=>5, :priority=>65535})
    @arrayhops = Array.new()
    @arrayhops.push(@hop)
    @arrayhops.push(@hop2)
  end
  
  it "append_hops_to_path" do

    Flow_manager.append_hops_to_path(@path,@arrayhops)

    hops = @path.hops
    expect( hops[0].datapath_id ).to eq( 0x1 )
    expect( hops[0].in_port ).to eq( 1 )
    expect( hops[0].out_port ).to eq( 2 )

    expect( hops[1].datapath_id ).to eq( 0x2 )
    expect( hops[1].in_port ).to eq( 2)
    expect( hops[1].out_port ).to eq( 1)
    actions = hops[1].actions

    actions[1] === SetEthSrcAddr
    expect( actions[1].mac_address.to_s ).to eq( "11:22:33:44:55:66" )
    actions[2] === SetEthDstAddr
    expect( actions[2].mac_address.to_s ).to eq( "11:22:33:44:55:66" )
    actions[3] === SetIpSrcAddr
    expect( actions[3].ip_address.to_s ).to eq( "192.168.1.1" )
    actions[4] === SetIpDstAddr
    expect( actions[4].ip_address.to_s ).to eq( "192.168.1.1" )
    actions[5] === SetIpTos
    expect( actions[5].type_of_service ).to eq( 32)
    actions[6] === SetTransportSrcPort
    expect( actions[6].port_number ).to eq( 5555 )
    actions[7] === SetTransportDstPort
    expect( actions[7].port_number ).to eq( 5555 )
    actions[8] === ActionSetVlanVid
    expect( actions[8].vlan_id ).to eq( 1)
    actions[9] === SetVlanPriority
    expect( actions[9].vlan_priority ).to eq( 7 )
    actions[10] === StripVlanHeader
    actions[11] === VendorAction
    expect( actions[11].vendor_id ).to eq( 0x00004cff )
    Array body = actions[11].body
    expect( body[0] ).to eq( "test" )
    expect( body[1] ).to eq( "test2" )
  end

    

  it "append_hops_to_path with single hop" do
    expect {Flow_manager.append_hops_to_path(@path,@hop)}.to raise_error(TypeError)
  end

  it "append_hops_to_path with mal second argument" do
    expect {Flow_manager.append_hops_to_path(@path,@path)}.to raise_error(TypeError)
  end

  it "append_hop_to_path with 1 hop" do
    Flow_manager.append_hop_to_path(@path,@hop)
    hops = @path.hops

    expect( hops[0].datapath_id ).to eq( 0x1 )
    expect( hops[0].in_port ).to eq( 1 )
    expect( hops[0].out_port ).to eq( 2 )
  end



  it "append_hop_to_path with 2 hops" do
    Flow_manager.append_hop_to_path(@path, @hop)
    Flow_manager.append_hop_to_path(@path, @hop2)
    
    hops = @path.hops
    expect( hops[0].datapath_id ).to eq( 0x1 )
    expect( hops[0].in_port ).to eq( 1 )
    expect( hops[0].out_port ).to eq( 2 )

    expect( hops[1].datapath_id ).to eq( 0x2 )
    expect( hops[1].in_port ).to eq( 2)
    expect( hops[1].out_port ).to eq( 1)
    actions = hops[1].actions

    actions[1] === SetEthSrcAddr
    expect( actions[1].mac_address.to_s ).to eq( "11:22:33:44:55:66" )
    actions[2] === SetEthDstAddr
    expect( actions[2].mac_address.to_s ).to eq( "11:22:33:44:55:66" )
    actions[3] === SetIpSrcAddr
    expect( actions[3].ip_address.to_s ).to eq( "192.168.1.1" )
    actions[4] === SetIpDstAddr
    expect( actions[4].ip_address.to_s ).to eq( "192.168.1.1" )
    actions[5] === SetIpTos
    expect( actions[5].type_of_service ).to eq( 32)
    actions[6] === SetTransportSrcPort
    expect( actions[6].port_number ).to eq( 5555 )
    actions[7] === SetTransportDstPort
    expect( actions[7].port_number ).to eq( 5555 )
    actions[8] === ActionSetVlanVid
    expect( actions[8].vlan_id ).to eq( 1)
    actions[9] === SetVlanPriority
    expect( actions[9].vlan_priority ).to eq( 7 )
    actions[10] === StripVlanHeader
    actions[11] === VendorAction
    expect( actions[11].vendor_id ).to eq( 0x00004cff )
    Array body = actions[11].body
    expect( body[0] ).to eq( "test" )
    expect( body[1] ).to eq( "test2" )
  end

  it "append_hop_to_path with array" do
    expect {Flow_manager.append_hop_to_path(@path,@arrayhops)}.to raise_error
  end

  it "append_hop_to_path with mal second argument" do
    match = Match.new()
    expect {Flow_manager.append_hop_to_path(@path,match)}.to raise_error(TypeError)
  end

  it "append_hop_to_path with mal first argument" do
    match = Match.new()
    expect {Flow_manager.append_hop_to_path(match,@hop2)}.to raise_error(TypeError)
  end
	
  after do
    puts "after do each"
    @hop = nil
    @hop2 = nil
    @path = nil
    @path2 = nil
    @arrayhops = nil
  end
end