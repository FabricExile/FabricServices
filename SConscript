#
# Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
#

Import('parentEnv')

env = parentEnv.CloneSubStage('Services')

if env['FABRIC_BUILD_OS'] == 'Darwin':
  env.Append(CXXFLAGS = ['-std=c++03'])
  env.Append(CXXFLAGS = ['-stdlib=libstdc++'])
  env.Append(CXXFLAGS = ['-fvisibility=hidden'])
  env.Append(LINKFLAGS = ['-stdlib=libstdc++'])

SConscript(
  dirs = [
    'Commands',
    'Persistence',
    'ASTWrapper',
    'CodeCompletion',
    'SplitSearch',
    ],
  exports = {'parentEnv': env}
  )

allServicesLibFiles = []
msvc_versions = ['12.0']
opt_versions = ['mt']
if env['FABRIC_BUILD_OS'] == 'Windows':

  # this cannot be done on the build machine
  # since it doesn't have VS 2010
  if 'servicesLib' in COMMAND_LINE_TARGETS:
    msvc_versions = ['10.0', '12.0']

  opt_versions = ['mt', 'md']

installHeaders = True
for msvc_version in msvc_versions:
  for opt_version in opt_versions:
    allServicesLibFiles += SConscript(
      'SConscript.ServicesLib',
      exports = {
        'parentEnv': env, 
        'dirs': [
          'Commands',
          'ASTWrapper',
          'Persistence',
          'CodeCompletion',
          # 'SplitSearch',
        ],
        'msvc_version': msvc_version,
        'opt_version': opt_version,
        'installHeaders': installHeaders
      },
      variant_dir = 'FabricServices-'+msvc_version+'-'+opt_version
      )
    installHeaders = False

Export('allServicesLibFiles')

env.Alias('servicesLib', allServicesLibFiles)
