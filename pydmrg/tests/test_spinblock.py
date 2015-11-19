import os, shutil
import pydmrg
from pydmrg import param
from pydmrg import spinblock
from pydmrg import stateinfo
from pydmrg import quanta

def make_dmrgenv():
    with pydmrg.capture_stdout():
        dmrg_tmpdir = 'dmrg_tmp1'
        if not os.path.exists(dmrg_tmpdir):
            os.mkdir(dmrg_tmpdir)
        dmrg_env = pydmrg.DMRGEnv()
        dmrg_env.scratch_prefix = dmrg_tmpdir
        dmrg_env.sym = 'd2h'
        dmrg_env.outputlevel = 1
        dmrg_env.sweep_iter_schedule   = [0     ,8     ,18    ]
        dmrg_env.sweep_state_schedule  = [20    ,50    ,100   ]
        dmrg_env.sweep_qstate_schedule = [0     ,0     ,0     ]
        dmrg_env.davidson_tol_schedule = [1e-6  ,1e-8  ,1e-8  ]
        dmrg_env.noise_schedule        = [1e-6  ,1e-7  ,1e-8  ]
        dmrg_env.additional_noise      = [0     ,0     ,0     ]
        dmrg_env.maxiter = 30
        dmrg_env.update_dmrginp('FCIDUMP-1')
    return dmrg_env

def test_q(si, ref):
    s = []
    for i,n in enumerate(si.quantaStates):
        q = si.get_quanta(i)
        d = '%d:%d:%d=%d' % (q.particleNumber, q.totalSpin, q.irrep, n)
        s.append(d == ref[i])
    return s

def test_InitStartingBlock1():
    dmrg_env = make_dmrgenv()
    s = quanta.SpinQuantum()
    irrep_id = 0
    s.init(dmrg_env.nelec, dmrg_env.spin, irrep_id)
    addstate = stateinfo.StateInfo(dmrg_env)
    addstate._raw.init_by_a_spinquantum(s._raw)
    dummyblock = spinblock.SpinBlock(dmrg_env)
    dummyblock.init_by_stateinfo(addstate)
    def new_blk():
        startingBlock = spinblock.SpinBlock(dmrg_env)
        startingBlock.init_dot(forward, start_id, forward_starting_size, is_complement=True)
        b = spinblock.SpinBlock(dmrg_env)
        b.default_op_components(False, startingBlock, dummyblock, \
                                haveNormops, haveCompops, dmrg_env.implicitTranspose)
        b.BuildSumBlock(param.NO_PARTICLE_SPIN_NUMBER_CONSTRAINT,
                        startingBlock, dummyblock)
        return b

    forward_starting_size = 1
    forward = True
    start_id = 0
    direct = False
    haveNormops = True
    haveCompops = True
    b = new_blk()
    print all(test_q(b.braStateInfo, ['4:0:0=1']))
    print all(test_q(b.ketStateInfo, ['4:0:0=1']))

    forward_starting_size = 2
    forward = False
    start_id = 1
    direct = True
    haveNormops = False
    haveCompops = False
    b = new_blk()
    print all(test_q(b.braStateInfo, ['4:0:0=1']))
    print all(test_q(b.ketStateInfo, ['4:0:0=1']))

def test_InitStartingBlock2():
    dmrg_env = make_dmrgenv()
    def new_blk():
        b = spinblock.SpinBlock(dmrg_env)
        b.default_op_components_compl(complementary, implicitTranspose)
        backwardSites = range(dmrg_env.tot_sites-backward_starting_size,
                              dmrg_env.tot_sites)
        b.BuildTensorProductBlock(backwardSites)
        return b

    backward_starting_size = 4
    complementary = False
    implicitTranspose = True
    b = new_blk()
    print b.braStateInfo.quantaStates == [1, 4, 10, 6, 20, 4, 20, 15, 1, 20, 4, 10, 6, 4, 1]
    print b.ketStateInfo.quantaStates == [1, 4, 10, 6, 20, 4, 20, 15, 1, 20, 4, 10, 6, 4, 1]

    backward_starting_size = 2
    complementary = True
    implicitTranspose = True
    b = new_blk()
    print b.braStateInfo.quantaStates == [1, 2, 3, 1, 2, 1]
    print b.ketStateInfo.quantaStates == [1, 2, 3, 1, 2, 1]

    backward_starting_size = 2
    complementary = False
    implicitTranspose = False
    b = new_blk()
    print b.braStateInfo.quantaStates == [1, 2, 3, 1, 2, 1]
    print b.ketStateInfo.quantaStates == [1, 2, 3, 1, 2, 1]

    backward_starting_size = 2
    complementary = True
    implicitTranspose = False
    b = new_blk()
    print all(test_q(b.braStateInfo, ['0:0:0=1', '1:1:0=2', '2:0:0=3',
                                      '2:2:0=1', '3:1:0=2', '4:0:0=1',]))
    print all(test_q(b.ketStateInfo, ['0:0:0=1', '1:1:0=2', '2:0:0=3',
                                      '2:2:0=1', '3:1:0=2', '4:0:0=1',]))


test_InitStartingBlock1()
test_InitStartingBlock2()
