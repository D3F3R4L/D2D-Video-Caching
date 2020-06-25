# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass

# def configure(conf):
#     conf.check_nonfatal(header_name='stdint.h', define_name='HAVE_STDINT_H')

def build(bld):
    module = bld.create_ns3_module('esba-dash-energy',['core', 'internet','wave', 'energy-module-lte'])
#    module = bld.create_ns3_module('dash-migration', ['core', 'internet','wave'])
    module.source = [
        'model/festive.cc',
        'model/panda.cc',
        'model/tobasco2.cc',
        'model/ESBA.cc',
        'model/tcp-stream-adaptation-algorithm.cc',
        'model/tcp-stream-client.cc',
        'model/tcp-stream-server.cc',
        'helper/tcp-stream-helper.cc'
        ]

  #  module_test = bld.create_ns3_module_test_library('v2v')
  #  module_test.source = [
  #      'test/v2v-test-suite.cc',
  #      ]

    headers = bld(features='ns3header')
    headers.module = 'esba-dash-energy'
    headers.source = [
        'model/festive.h',
        'model/panda.h',
        'model/tobasco2.h',
        'model/ESBA.h',
        'model/tcp-stream-adaptation-algorithm.h',
        'model/tcp-stream-client.h',
        'model/tcp-stream-server.h',
        'model/tcp-stream-interface.h',
        'helper/tcp-stream-helper.h'
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')

    # bld.ns3_python_bindings()


