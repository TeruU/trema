Feature: flow_manager2 tests

  As a Trema user
  I want to use flow_manager to make a flow to OpenFlow switches
  So that I can debug my Trema apps
  
  Background:
  Given a file named "flow_manager.conf" with:
    """
    vswitch("switch1ForFM") { datapath_id "0x1" }
    vswitch("switch2ForFM") { datapath_id "0x2" }
    
    link 0x1,0x2
    
    event :port_status => "FlowManagerController", :packet_in => "FlowManagerController", :state_notify => "FlowManagerController"
    """
    
  Given a file named "cflow_manager.conf" with:
    """
    vswitch("switch1ForFM") { datapath_id "0x1" }
    vswitch("switch2ForFM") { datapath_id "0x2" }
    
    link 0x1,0x2
    
    event :port_status => "flow_manager_test", :packet_in => "flow_manager_test", :state_notify => "flow_manager_test"
    """
  @long_timeout  
  Scenario: Run "flow_manager c test" C example
    Given I run `trema run -c cflow_manager.conf ../../objects/examples/flow_manager_example/flow_manager_example`
    Then the file "../../tmp/log/flow_manager_example.log" should contain " *** Path setup completed ( status = succeeded, user_data = (nil) )"
    
  @long_timeout    
  Scenario: Run "flow_manager path setup test" Ruby example
    Given I run `trema run ../../src/flow_manager/flow_manager_test/flow_manager_path_setup_test.rb -c flow_manager.conf -d`
    When *** sleep 15 ***
    Then the file "../../tmp/log/FlowManagerController.log" should contain "**** Path setup completed ( status = succeeded)***"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "path.priority:65535"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "path.idle:5"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "path.hard_timeout:30"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "path.match:wildcards = 0x3820ff(all), in_port = 0, dl_src = 00:00:00:00:00:00, dl_dst = 00:00:00:00:00:00, dl_vlan = 0, dl_vlan_pcp = 0, dl_type = 0, nw_tos = 0, nw_proto = 0, nw_src = 0.0.0.0/0, nw_dst = 0.0.0.0/0, tp_src = 0, tp_dst = 0"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "arrHops[0].datapath_id:1"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "arrHops[0].in_port:1"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "arrHops[0].out_port:2"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "arrAction1[0].port_number():1"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "arrHops[1].datapath_id:2"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "arrHops[1].in_port:2"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "arrHops[1].out_port:3"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "arrHops[1].actions:false"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "***Path teardown completed ( reason = timeout)***"

  
  @long_timeout 
  Scenario: Run "flow_manager path setup duplicate" Ruby example
    Given I run `trema run ../../src/flow_manager/flow_manager_test/flow_manager_path_setup_duplicate_test.rb -c flow_manager.conf -d`
    And *** sleep 10 ***
    Then the file "../../tmp/log/FlowManagerController.log" should contain "**** Path setup completed ( status = conflicted entry)***"
  
  @long_timeout      
  Scenario: Run "flow_manager path teardown by manual" Ruby example
    Given I run `trema run ../../src/flow_manager/flow_manager_test/flow_manager_path_teardown_by_manual_test.rb -c flow_manager.conf -d`
    And *** sleep 10 ***
    Then the file "../../tmp/log/FlowManagerController.log" should contain " ***Path teardown completed ( reason = manually requested)***"
  
  @long_timeout    
  Scenario: Run "flow_manager module setup" Ruby example
    Given I run `trema run ../../src/flow_manager/flow_manager_test/flow_manager_module_setup_test.rb -c flow_manager.conf -d`
    And *** sleep 15 ***
    Then the file "../../tmp/log/FlowManagerController.log" should contain "**** Path setup completed ( status = succeeded)***"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "path.priority:65535"
    Then the file "../../tmp/log/FlowManagerController.log" should contain " path.idle:5"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "path.hard_timeout:30"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "path.match:wildcards = 0x3820fe(dl_src|dl_dst|dl_type|dl_vlan|dl_vlan_pcp|nw_proto|nw_tos|nw_src(32)|nw_dst(32)|tp_src|tp_dst), in_port = 1, dl_src = 00:00:00:00:00:00, dl_dst = 00:00:00:00:00:00, dl_vlan = 0, dl_vlan_pcp = 0, dl_type = 0, nw_tos = 0, nw_proto = 0, nw_src = 0.0.0.0/0, nw_dst = 0.0.0.0/0, tp_src = 0, tp_dst = 0"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "arrHops[0].datapath_id:1"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "arrHops[0].in_port:1"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "arrHops[0].out_port:2"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "arrAction1[1].max_len():65535"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "arrHops[1].datapath_id:2"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "arrHops[1].in_port:2"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "arrHops[1].out_port:1"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "arrHops[1].actions:false"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "***Path teardown completed ( reason = timeout)***"
  
  @long_timeout  
  Scenario: Run "flow_manager module lookup" Ruby example
    Given I run `trema run ../../src/flow_manager/flow_manager_test/flow_manager_module_lookup_test.rb -c flow_manager.conf -d`
    And *** sleep 15 ***
    Then the file "../../tmp/log/FlowManagerController.log" should contain "**** Path setup completed ( status = succeeded)***"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "***lookup start"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "path.priority:65535"
    Then the file "../../tmp/log/FlowManagerController.log" should contain " path.idle:5"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "path.hard_timeout:30"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "path.match:wildcards = 0x3820fe(dl_src|dl_dst|dl_type|dl_vlan|dl_vlan_pcp|nw_proto|nw_tos|nw_src(32)|nw_dst(32)|tp_src|tp_dst), in_port = 1, dl_src = 00:00:00:00:00:00, dl_dst = 00:00:00:00:00:00, dl_vlan = 0, dl_vlan_pcp = 0, dl_type = 0, nw_tos = 0, nw_proto = 0, nw_src = 0.0.0.0/0, nw_dst = 0.0.0.0/0, tp_src = 0, tp_dst = 0"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "***Path teardown completed ( reason = timeout)***"
  
  @long_timeout 
  Scenario: Run "flow_manager module teardown" Ruby example
    Given I run `trema run ../../src/flow_manager/flow_manager_test/flow_manager_module_teardown_test.rb -c flow_manager.conf -d`
    And *** sleep 10 ***
    Then the file "../../tmp/log/FlowManagerController.log" should contain "**** Path setup completed ( status = succeeded)***"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "teardown bool:true"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "***Path teardown completed ( reason = manually requested)***"    
  
  @long_timeout  
  Scenario: Run "flow_manager module teardown by match" Ruby example
    Given I run `trema run ../../src/flow_manager/flow_manager_test/flow_manager_module_teardown_by_match_test.rb -c flow_manager.conf -d`
    And *** sleep 10 ***
    Then the file "../../tmp/log/FlowManagerController.log" should contain "**** Path setup completed ( status = succeeded)***"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "teardown bool:true"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "***Path teardown completed ( reason = manually requested)***"    
    
  @long_timeout      
  Scenario: Run "flow_manager_init_finalize_test" Ruby example
    Given I run `trema run ../../src/flow_manager/flow_manager_test/flow_manager_init_finalize_test.rb -c flow_manager.conf -d`
    And *** sleep 15 ***
    Then the file "../../tmp/log/FlowManagerController.log" should contain "true"
    Then the file "../../tmp/log/FlowManagerController.log" should contain "false"
