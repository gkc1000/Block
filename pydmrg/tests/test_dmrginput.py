import os
import pydmrg


def make_dmrgenv():
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

with pydmrg.capture_stdout() as stdout:
    make_dmrgenv()
    out = stdout.read().splitlines()

print out[3].strip()  == 'Fiedler-vector orbital ordering: 3 4 5 6 7 8 1 2'
print out[7].strip()  == 'Initial HF occupancy guess: 0 0 0 0 0 0 0 0 1 1 0 0 1 1 0 0'
print out[11].strip() == 'Total number of orbitals                           :   8'
print out[12].strip() == 'Symmetry of the targeted wavefunctions             :   4:0:1'
print out[13].strip() == 'Number of wavefunctions targeted                   :   1'
print out[14].strip() == 'Symmetry of the molecule                           :   d2h'
print out[15].strip() == 'Irreducible representations of the orbitals        :   Ag  Ag  Ag  Ag  Ag  Ag  Ag  Ag'
