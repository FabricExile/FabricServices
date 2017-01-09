#
# Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#

import os
import platform

AddOption('--buildType',
    dest='buildType',
    type='string',
    nargs=1,
    action='store',
    help='Type of build to perform (Release or Debug)')

if not os.environ.has_key('FABRIC_DIR'):
    raise Exception("No FABRIC_DIR environment variable specified.")

buildOS = 'Darwin'
if platform.system().lower().startswith('win'):
    buildOS = 'Windows'
elif platform.system().lower().startswith('lin'):
    buildOS = 'Linux'

buildArch = 'x86_64'
buildType = 'Release'

if str(GetOption('buildType')).lower() == 'debug':
    buildType = 'Debug'

msvc_version = None
opt_version = None
if buildOS == 'Windows':
    msvc_version = '12.0'
    opt_version = 'mt'
install_headers = True

env = Environment(MSVC_VERSION = msvc_version)
env.Append(CPPPATH = [env.Dir('#').srcnode().abspath])

env['FABRIC_BUILD_OS'] = buildOS
env['FABRIC_BUILD_ARCH'] = buildArch
env['FABRIC_BUILD_TYPE'] = buildType
env['FABRIC_VERSION_MAJ'] = '2'
env['FABRIC_VERSION_MIN'] = '0'
env['FABRIC_VERSION_REV'] = '1'

if buildOS == 'Linux':
    env.Append(CXXFLAGS = ['-fPIC'])
elif buildOS == 'Windows':
    env.Append(CCFLAGS = ['/Od', '/Zi']) # 'Z7'
    env['CCPDBFLAGS']    = ['${(PDB and "/Fd%s.pdb /Zi" % File(PDB)) or ""}']
    env.Append(CCFLAGS = ['/FS'])

fabricDir = os.environ['FABRIC_DIR']
capiSharedLibFlags = {
    'CPPDEFINES': ['FEC_SHARED'],
    'CPPPATH': [
        os.path.join(fabricDir, 'include'),
    ],
    'LIBPATH': [os.path.join(fabricDir, 'lib')],
    'LIBS': ['FabricCore-2.0']
}

variant_dir = 'FabricServices'
if buildOS == 'Windows':
    variant_dir = 'FabricServices-'+msvc_version+'-'+opt_version

servicesLib = SConscript(
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
    'installHeaders': install_headers,
    'stageDir': env.Dir('#').Dir('stage'),
    'capiSharedLibFlags': capiSharedLibFlags,
    'llvmFlags': {},
  },
  variant_dir = env.Dir('#').Dir('build').Dir(variant_dir)
  )
installHeaders = False

env.Default(servicesLib)
