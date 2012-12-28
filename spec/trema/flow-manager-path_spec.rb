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

describe Hop, ".new" do

  before do
    datapath_id = 0x1;
    in_port = 1;
    out_port = 2;
    hop = Hop.new(datapath_id, in_port, out_port)

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
                      VendorAction.new( 0x00004cff, Array["test", "test2"] ) ] )
    hop2 = Hop.new(datapath_id2, in_port2, out_port2, actions2)

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
    Flow_manager.append_hop_to_path(@path,hop)
    Flow_manager.append_hop_to_path(@path,hop2)
    @path2 = Path.new(match2, options={:idle_timeout=>5, :hard_timeout=>5, :priority=>65535})
  end

  it "single argument" do
    match = Match.new()
    Path.new(match).instance_of?(Path).should == true
  end

  it "two arguments" do
    match = Match.new()
    Path.new(match, options={:idle_timeout=>5, :hard_timeout=>5, :priority=>65535}).instance_of?(Path).should == true
  end

  it "three arguments" do
    match = Match.new()
    expect {Path.new(match, options={:idle_timeout=>5, :hard_timeout=>5, :priority=>65535}, 1)}.to raise_error() 
  end

  it "minus arguments" do
    match = Match.new()
    expect {Path.new(match, options={:idle_timeout=>-1, :hard_timeout=>5, :priority=>65535})}.to raise_error() 
  end

  it "minus arguments" do
    match = Match.new()
    expect {Path.new(match, options={:idle_timeout=>1, :hard_timeout=>-5, :priority=>65535})}.to raise_error() 
  end

  it "minus arguments" do
    match = Match.new()
    expect {Path.new(match, options={:idle_timeout=>1, :hard_timeout=>5, :priority=>-65535})}.to raise_error() 
  end

  it "bad arguments" do
    match = Match.new()
    expect {Path.new(match, 1)}.to raise_error("The second argument should be hash.") 
  end

  it "path" do

    hopArray = @path.hops
    hopArray[0].datapath_id.should == 0x1
    hopArray[0].in_port.should == 1
    hopArray[0].out_port.should == 2
    hopArray[1].datapath_id.should == 0x2
    hopArray[1].in_port.should == 2
    hopArray[1].out_port.should == 1
    hopArray[2].should be_nil

  end



  it "priority" do

    @path.priority.should == 65535
    @path2.priority.should == 65535

  end




  it "hard_timeout" do

    @path.hard_timeout.should == 5
    @path2.hard_timeout.should == 5

  end




  it "idle_timeout" do

    @path.idle_timeout.should == 5
    @path2.idle_timeout.should == 5

  end



  it "p_match" do

      passed_match1 = @path.match
      match = Match.new()
      passed_match1.compare(match).should == true

      passed_match2 = @path2.match
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
      passed_match2.compare(match2).should == true

  end


  after do
  	@path = nil
    @path2 = nil
  end
end

### Local variables:
### mode: Ruby
### coding: utf-8-unix
### indent-tabs-mode: nil
### End:
