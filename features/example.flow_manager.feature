Feature: flow_manager tests

  As a Trema user
  I want to use flow_manager to make a flow to OpenFlow switches
  So that I can debug my Trema apps
    
  Scenario: run flow_manager_test -c flow_manager_test.conf

    When I try trema run "./objects/examples/flow_manager_test/flow_manager_test" with following configuration (backgrounded):
      """
		vswitch("switch1ForFM") { datapath_id "0x1" }
		vswitch("switch2ForFM") { datapath_id "0x2" }
		
		link 0x1,0x2
		
		run {
		  path "./objects/flow_manager/flow_manager"
		}
		
		event :port_status => "flow_manager_test", :packet_in => "flow_manager_test", :state_notify => "flow_manager_test"
      """
    And *** sleep 15 ***
    Then the output should include:
    	"""
    	Path: match = [wildcards = 0x3fffff(all), in_port = 0, dl_src = 00:00:00:00:00:00, dl_dst = 00:00:00:00:00:00, dl_vlan = 0, dl_vlan_pcp = 0, dl_type = 0, nw_tos = 0, nw_proto = 0, nw_src = 0.0.0.0/0, nw_dst = 0.0.0.0/0, tp_src = 0, tp_dst = 0], priority = 65535, idle_timeout = 10, hard_timeout = 20, n_hops = 2.
		Hop: datapath_id = 0x1, in_port = 1, out_port = 2, n_actions = 11.
		Hop: datapath_id = 0x2, in_port = 2, out_port = 1, n_actions = 0.
		*** Path setup completed ( status = succeeded, user_data = (nil) ) *** 
		*** Path teardown completed ( reason = timeout, user_data = (nil) ) *** 
      	"""
    
  Scenario: run flow_manager_module_setup_test.rb -c flow_manager.conf

    When I try trema run "./src/examples/flow_manager_test/flow_manager_module_setup_test.rb" with following configuration (backgrounded):
      """
		vswitch("switch1ForFM") { datapath_id "0x1" }
		vswitch("switch2ForFM") { datapath_id "0x2" }
		
		link 0x1,0x2
		
		run {
		  path "./objects/flow_manager/flow_manager"
		}
		
		event :port_status => "FlowManagerController", :packet_in => "FlowManagerController", :state_notify => "FlowManagerController"
      """
    And *** sleep 15 ***
    And I try to run "./trema dump_flows switch1ForFM" (log = "switch1ForFM.log")
    Then the output should include:
      """
      status:succeeded
      path.priority:65535
      path.idle:5
      path.hard_timeout:30
      path.match:wildcards = 0x3820fe(dl_src|dl_dst|dl_type|dl_vlan|dl_vlan_pcp|nw_proto|nw_tos|nw_src(32)|nw_dst(32)|tp_src|tp_dst), in_port = 1, dl_src = 00:00:00:00:00:00, dl_dst = 00:00:00:00:00:00, dl_vlan = 0, dl_vlan_pcp = 0, dl_type = 0, nw_tos = 0, nw_proto = 0, nw_src = 0.0.0.0/0, nw_dst = 0.0.0.0/0, tp_src = 0, tp_dst = 0
      arrHops[0].datapath_id:1
      arrHops[0].in_port:1
      arrHops[0].out_port:2
      arrHops[1].datapath_id:2
      arrHops[1].in_port:2
      arrHops[1].out_port:1
      arrHops[1].actions:false
      reason:timeout
      """
      
  Scenario: run flow_manager_module_lookup_test.rb -c flow_manager.conf

    When I try trema run "./src/examples/flow_manager_test/flow_manager_module_lookup_test.rb" with following configuration (backgrounded):
      """
		vswitch("switch1ForFM") { datapath_id "0x1" }
		vswitch("switch2ForFM") { datapath_id "0x2" }
		
		link 0x1,0x2
		
		run {
		  path "./objects/flow_manager/flow_manager"
		}
		
		event :port_status => "FlowManagerController", :packet_in => "FlowManagerController", :state_notify => "FlowManagerController"
      """
    And *** sleep 15 ***
    Then the output should include:
    	"""
		status:succeeded
		path.match:wildcards = 0x3820fe(dl_src|dl_dst|dl_type|dl_vlan|dl_vlan_pcp|nw_proto|nw_tos|nw_src(32)|nw_dst(32)|tp_src|tp_dst), in_port = 1, dl_src = 00:00:00:00:00:00, dl_dst = 00:00:00:00:00:00, dl_vlan = 0, dl_vlan_pcp = 0, dl_type = 0, nw_tos = 0, nw_proto = 0, nw_src = 0.0.0.0/0, nw_dst = 0.0.0.0/0, tp_src = 0, tp_dst = 0
		path.priority:65535
		path.idle:5
		path.hard_timeout:30
		path.match:wildcards = 0x3820fe(dl_src|dl_dst|dl_type|dl_vlan|dl_vlan_pcp|nw_proto|nw_tos|nw_src(32)|nw_dst(32)|tp_src|tp_dst), in_port = 1, dl_src = 00:00:00:00:00:00, dl_dst = 00:00:00:00:00:00, dl_vlan = 0, dl_vlan_pcp = 0, dl_type = 0, nw_tos = 0, nw_proto = 0, nw_src = 0.0.0.0/0, nw_dst = 0.0.0.0/0, tp_src = 0, tp_dst = 0
		reason:timeout
      	"""
  Scenario: run flow_manager_module_teardown_test.rb -c flow_manager.conf

    When I try trema run "./src/examples/flow_manager_test/flow_manager_module_teardown_test.rb" with following configuration (backgrounded):
      """
		vswitch("switch1ForFM") { datapath_id "0x1" }
		vswitch("switch2ForFM") { datapath_id "0x2" }
		
		link 0x1,0x2
		
		run {
		  path "./objects/flow_manager/flow_manager"
		}
		
		event :port_status => "FlowManagerController", :packet_in => "FlowManagerController", :state_notify => "FlowManagerController"
      """
    And *** sleep 15 ***
    Then the output should include:
    	"""
		status:succeeded
		teardown bool:true
		reason:manually requested
      	"""
      	
    Scenario: run flow_manager_module_teardown_by_match_test.rb -c flow_manager.conf

    When I try trema run "./src/examples/flow_manager_test/flow_manager_module_teardown_by_match_test.rb" with following configuration (backgrounded):
      """
		vswitch("switch1ForFM") { datapath_id "0x1" }
		vswitch("switch2ForFM") { datapath_id "0x2" }
		
		link 0x1,0x2
		
		run {
		  path "./objects/flow_manager/flow_manager"
		}
		
		event :port_status => "FlowManagerController", :packet_in => "FlowManagerController", :state_notify => "FlowManagerController"
      """
    And *** sleep 15 ***
    Then the output should include:
    	"""
		status:succeeded
		teardown bool:true
		reason:manually requested
      	"""
      	