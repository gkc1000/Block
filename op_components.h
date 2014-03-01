/*                                                                           
Developed by Sandeep Sharma and Garnet K.-L. Chan, 2012                      
Copyright (c) 2012, Garnet K.-L. Chan                                        
                                                                             
This program is integrated in Molpro with the permission of 
Sandeep Sharma and Garnet K.-L. Chan
*/


#ifndef SPIN_OP_COMPONENTS_H
#define SPIN_OP_COMPONENTS_H
#include <stdio.h>
#include <boost/function.hpp>
#include <boost/functional.hpp>
#include <boost/format.hpp>
#include <para_array.h>
#include <boost/shared_ptr.hpp>
#include <list>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include "Operators.h"
#include "operatorloops.h"
#include <string>
//FIXME
#include "two_index_ops.h"
#include "three_index_ops.h"
#include "four_index_ops.h"
#include "para_array_3d.h"
#include "para_array_4d.h"

namespace SpinAdapted{

//FIXME why do we need this declaration?
//class SpinBlock;

//===========================================================================================================================================================
// Choose the type of array for different types of Operators

template <class T> struct ChooseArray {
  typedef para_array_1d<std::vector<boost::shared_ptr<SparseMatrix> > > ArrayType;
};
template <> struct ChooseArray<Cre> {
  typedef para_array_1d<std::vector<boost::shared_ptr<Cre> > > ArrayType;
};
template <> struct ChooseArray<CreDes> {
  typedef para_array_triang_2d<std::vector<boost::shared_ptr<CreDes> > > ArrayType;
};
template <> struct ChooseArray<CreCre> {
  typedef para_array_triang_2d<std::vector<boost::shared_ptr<CreCre> > > ArrayType;
};
template <> struct ChooseArray<CreDesComp> {
  typedef para_array_triang_2d<std::vector<boost::shared_ptr<CreDesComp> > > ArrayType;
};
template <> struct ChooseArray<DesDesComp> {
  typedef para_array_triang_2d<std::vector<boost::shared_ptr<DesDesComp> > > ArrayType;
};
template <> struct ChooseArray<CreCreDesComp> {
  typedef para_array_1d<std::vector<boost::shared_ptr<CreCreDesComp> > > ArrayType;
};
template <> struct ChooseArray<Ham> {
  typedef para_array_0d<std::vector<boost::shared_ptr<Ham> > > ArrayType;
};
//FIXME move these into a different header file?
//MAW NPDM >>>>>
template <> struct ChooseArray<RI3index> {
  typedef para_array_3d<std::vector<boost::shared_ptr<RI3index> > > ArrayType;
};
template <> struct ChooseArray<RI4index> {
  typedef para_array_4d<std::vector<boost::shared_ptr<RI4index> > > ArrayType;
};
template <> struct ChooseArray<Des> {
  typedef para_array_1d<std::vector<boost::shared_ptr<Des> > > ArrayType;
};
template <> struct ChooseArray<DesDes> {
  typedef para_array_triang_2d<std::vector<boost::shared_ptr<DesDes> > > ArrayType;
};
template <> struct ChooseArray<DesDesDes> {
  typedef para_array_3d<std::vector<boost::shared_ptr<DesDesDes> > > ArrayType;
};
//MAW 3PDM >>>>>
template <> struct ChooseArray<DesCre> {
  typedef para_array_triang_2d<std::vector<boost::shared_ptr<DesCre> > > ArrayType;
};
template <> struct ChooseArray<CreCreDes> {
  typedef para_array_3d<std::vector<boost::shared_ptr<CreCreDes> > > ArrayType;
};
template <> struct ChooseArray<CreDesDes> {
  typedef para_array_3d<std::vector<boost::shared_ptr<CreDesDes> > > ArrayType;
};
template <> struct ChooseArray<CreDesCre> {
  typedef para_array_3d<std::vector<boost::shared_ptr<CreDesCre> > > ArrayType;
};
template <> struct ChooseArray<CreCreCre> {
  typedef para_array_3d<std::vector<boost::shared_ptr<CreCreCre> > > ArrayType;
};
//MAW 4PDM >>>>>
template <> struct ChooseArray<DesCreDes> {
  typedef para_array_3d<std::vector<boost::shared_ptr<DesCreDes> > > ArrayType;
};
template <> struct ChooseArray<DesDesCre> {
  typedef para_array_3d<std::vector<boost::shared_ptr<DesDesCre> > > ArrayType;
};
template <> struct ChooseArray<DesCreCre> {
  typedef para_array_3d<std::vector<boost::shared_ptr<DesCreCre> > > ArrayType;
};
template <> struct ChooseArray<CreCreDesDes> {
  typedef para_array_4d<std::vector<boost::shared_ptr<CreCreDesDes> > > ArrayType;
};
template <> struct ChooseArray<CreDesCreDes> {
  typedef para_array_4d<std::vector<boost::shared_ptr<CreDesCreDes> > > ArrayType;
};
template <> struct ChooseArray<CreDesDesCre> {
  typedef para_array_4d<std::vector<boost::shared_ptr<CreDesDesCre> > > ArrayType;
};
template <> struct ChooseArray<CreDesDesDes> {
  typedef para_array_4d<std::vector<boost::shared_ptr<CreDesDesDes> > > ArrayType;
};
template <> struct ChooseArray<CreCreCreDes> {
  typedef para_array_4d<std::vector<boost::shared_ptr<CreCreCreDes> > > ArrayType;
};
template <> struct ChooseArray<CreCreDesCre> {
  typedef para_array_4d<std::vector<boost::shared_ptr<CreCreDesCre> > > ArrayType;
};
template <> struct ChooseArray<CreDesCreCre> {
  typedef para_array_4d<std::vector<boost::shared_ptr<CreDesCreCre> > > ArrayType;
};
template <> struct ChooseArray<CreCreCreCre> {
  typedef para_array_4d<std::vector<boost::shared_ptr<CreCreCreCre> > > ArrayType;
};
//MAW NPDM <<<<<

//===========================================================================================================================================================

class Op_component_base
{
 private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & m_core & m_deriv;
  }
 protected:
  bool m_core;
  bool m_deriv;
 public:
  virtual void build_iterators(SpinBlock& b)=0;
  virtual void build_operators(SpinBlock& b, std::string& ofile, std::string& sysfile, std::string& dotfile) = 0;
  virtual void build_csf_operators(SpinBlock& b, std::vector< Csf >& c, std::vector< std::vector<Csf> >& ladders) = 0;
  virtual void renormalise_transform(const std::vector<Matrix>& rotateMatrix, const StateInfo* s) =0;
  //virtual string type_name() = 0;
  virtual int get_size() const =0;
  virtual int size() const=0;
  virtual void clear() =0;
  virtual std::vector<boost::shared_ptr<SparseMatrix> > get_local_element(int i) =0;
  virtual std::vector<boost::shared_ptr<SparseMatrix> > get_global_element(int i)=0;
  virtual const std::vector< int >& get_local_indices() =0;
  const bool &is_core() const {return m_core;}
  const bool &is_deriv() const {return m_deriv;}
  void set_core(bool is_core) {m_core = is_core;}
  virtual void add_local_indices(int i, int j=-1) {};
  virtual bool is_local() const = 0;
  virtual bool& set_local() = 0; 
  virtual std::vector< std::vector<int> > get_array() const =0;
  virtual const std::vector<boost::shared_ptr<SparseMatrix> > get_element(int i, int j=-1, int k=-1, int l=-1) const = 0;
  virtual std::vector<boost::shared_ptr<SparseMatrix> > get_element(int i, int j=-1, int k=-1, int l=-1) = 0;
  virtual bool has(int i, int j=-1, int k=-1, int l=-1) const = 0;
  virtual bool has_local_index(int i, int j=-1, int k=-1, int l=-1) const = 0;
  virtual boost::shared_ptr<SparseMatrix> get_op_rep(const SpinQuantum& s, int i=-1, int j=-1, int k=-1, int l=-1) = 0;
  virtual const boost::shared_ptr<SparseMatrix> get_op_rep(const SpinQuantum& s, int i=-1, int j=-1, int k=-1, int l=-1) const = 0;
//MAW
  virtual boost::shared_ptr<SparseMatrix> get_op_rep(const std::vector<SpinQuantum>& s, int i=-1, int j=-1, int k=-1, int l=-1) = 0;
  virtual const boost::shared_ptr<SparseMatrix> get_op_rep(const std::vector<SpinQuantum>& s, int i=-1, int j=-1, int k=-1, int l=-1) const = 0;
  virtual std::string get_op_string() const = 0;
  virtual std::string get_filename() const = 0;
  virtual ~Op_component_base() {}  
};

//===========================================================================================================================================================

template <class Op> class Op_component : public Op_component_base
{
 private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & boost::serialization::base_object<Op_component_base>(*this);
    ar.register_type(static_cast<Op *>(NULL));
    ar & m_op & uniqueID;
  }

 protected:
  typedef typename ChooseArray<Op>::ArrayType paraarray;
  typedef Op OpType; 
  paraarray m_op;
//FIXME this uniquely labels wrt op_components of the same type (e.g. CRECRECRE) but won't distinguish e.g. CRECRE and CREDES on same spinblock.
  int uniqueID;
//MAW use for unique filename for disk-based operator storage 
  static int nIDgenerator; // (this is just declaration; note definition below!)

 public:
  std::string get_op_string() const;
  Op_component() { m_deriv=false; uniqueID = nIDgenerator++; }
  Op_component(bool core) { m_core=core; m_deriv=false; uniqueID = nIDgenerator++;}
  bool& set_local() {return m_op.set_local();}
  bool is_local() const {return m_op.is_local();}
  //FIXME >>>>>>>>>>>>>>>>>>>>
  //FIXME the names of these functions are very misleading!  One is local, one is global!!!
  int get_size() const {return m_op.local_nnz();}
  int size() const  {return m_op.global_nnz();}
  //FIXME <<<<<<<<<<<<<<<<<<<<<
  bool has(int i, int j=-1, int k=-1, int l=-1) const {return m_op.has(i, j, k, l);}
  bool has_local_index(int i, int j=-1, int k=-1, int l=-1) const {return m_op.has_local_index(i, j, k, l);}
  virtual void add_local_indices(int i, int j=-1){};
  void clear(){m_op.clear();}
  void build_iterators(SpinBlock& b);
  const std::vector< int >& get_local_indices() { return m_op.get_local_indices(); }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------
//MAW use for unique filename for disk-based operator storage -- note we need optype prefix!
  std::string get_filename() const 
  { 
    std::string file;
    file = str( boost::format("%s%s%s%s%d%s%d%s") % dmrginp.load_prefix() % "/" % get_op_string() % "_" % uniqueID % "_p" % mpigetrank() % ".tmp" ); 
    return file;
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

  void build_csf_operators(SpinBlock& b, std::vector< Csf >& c, std::vector< std::vector<Csf> >& ladders) 
  { 
    if ( (m_op.num_indices() > 2) && ( ! dmrginp.do_npdm_in_core()) ) {
      // Build on disk (assume we are building from scratch)
      std::string ofile = get_filename();
      std::ofstream ofs(ofile.c_str(), std::ios::binary);
      for_all_operators_to_disk( *this, b, ofs, bind(&SparseMatrix::buildUsingCsf, _1,boost::ref(b), boost::ref(ladders), boost::ref(c)) );
      ofs.close();
      // DEBUG only: now read back into core
      if (false) { 
        std::ifstream ifs(ofile.c_str(), std::ios::binary);
        for_all_operators_multithread( *this, bind(&SparseMatrix::read_from_disk, _1, boost::ref(ifs)) );
        ifs.close();
      }
    }
    else {
      // Build in core
      for_all_operators_multithread( *this, bind(&SparseMatrix::buildUsingCsf, _1, boost::ref(b), boost::ref(ladders), boost::ref(c)) );
    }
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------
//FIXME remove string parameters
  void build_operators(SpinBlock& b, std::string& ofile, std::string& sysfile, std::string& dotfile) 
  { 
    // Use alternative drivers to build n-index operators for n>2
    if ( m_op.num_indices() > 2 ) 
      assert(false);
//    if ( (m_op.num_indices() == 3) && ( ! dmrginp.do_npdm_in_core()) ) {
//      // Build on disk (reading from disk, as necessary)
//      std::ofstream ofs(ofile.c_str(), std::ios::binary);
//      std::ifstream sysfs(sysfile.c_str(), std::ios::binary);
//      std::ifstream dotfs(dotfile.c_str(), std::ios::binary);
//      for_all_operators_to_disk( *this, b, ofs, bind(&SparseMatrix::build_from_disk, _1, boost::ref(b), boost::ref(sysfs), boost::ref(dotfs)) );
//      ofs.close();
//      sysfs.close();
//      dotfs.close();
//      // DEBUG only: now read back into core
//      if (false) { 
//        std::ifstream ifs(ofile.c_str(), std::ios::binary);
//        for_all_operators_multithread( *this, bind(&SparseMatrix::read_from_disk, _1, boost::ref(ifs)) );
//        ifs.close();
//      }
//    }
    else {
      // Build in core
      singlethread_build(*this, b); 
    }
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

  void renormalise_transform_on_disk(const std::vector<Matrix>& rotateMatrix, const StateInfo* stateinfo) 
  {
    // Open filesystem if necessary
    std::ifstream ifs;
    std::ofstream ofs;
    std::string ifile = get_filename();
pout << "renormalize on disk; ofile = " << ifile << endl;
    std::string ofile = ifile + ".renorm";
    if ( ! dmrginp.do_npdm_in_core() ) {
      ifs.open( ifile.c_str(), std::ios::binary );
      ofs.open( ofile.c_str(), std::ios::binary );
    }
  
    // Loop over all operator indices
    std::vector<boost::shared_ptr<SparseMatrix> > spin_ops;
    for (int idx = 0; idx < get_size(); ++idx) {
      if ( dmrginp.do_npdm_in_core() ) {
        // Get from core
        spin_ops = get_local_element(idx);
      }
      else {
        // Get from disk
        int size = get_local_element(0).size();
        assert( (size == 3) || (size == 6) );
        spin_ops.clear();
        // Read in all spin components for this set of spatial indices
        for ( int i=0; i<size; i++) {
          boost::shared_ptr<SparseMatrix> op (new Cre);
          boost::archive::binary_iarchive load_op(ifs);
          load_op >> *op;
          assert( op->get_built_on_disk() );
          spin_ops.push_back(op);
        }
      }
  
      // Loop over spin-op components
      for (int jdx=0; jdx < spin_ops.size(); jdx++) {
int i = spin_ops[jdx]->get_orbs()[0];
int j = spin_ops[jdx]->get_orbs()[1];
int k = spin_ops[jdx]->get_orbs()[2];
pout << "i,j,k = " << i << "," << j << "," << k << endl;
        assert( spin_ops[jdx]->get_built() );
        // Renormalize
        spin_ops[jdx]->renormalise_transform( rotateMatrix, stateinfo );
        // Move back to disk if necessary
        if ( ! dmrginp.do_npdm_in_core() ) {
          spin_ops[jdx]->set_built_on_disk() = true;
          boost::archive::binary_oarchive save_op(ofs);
          save_op << *spin_ops[jdx];
        }
      }
    }
  
    // Close filesystem if necessary
    if ( ! dmrginp.do_npdm_in_core() ) {
      ifs.close();
      ofs.close();
      int result = rename( ofile.c_str(), ifile.c_str() );
      assert( result == 0 );
    }
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

  void renormalise_transform(const std::vector<Matrix>& rotateMatrix, const StateInfo* s) 
  {
    if ( m_op.num_indices() > 2 ) {
      // For operators built on disk or in core
      renormalise_transform_on_disk( rotateMatrix, s );
    }
//    if ( (m_op.num_indices() == 3) && ( ! dmrginp.do_npdm_in_core()) ) {
//      // Build on disk (load, renormalize, save)
//      std::string ifile = get_filename();
//cout << "renormalizing 3-index operators, ofile = " << ifile << endl;
//      std::string ofile = get_filename() + ".renorm";
//      std::ifstream ifs(ifile.c_str(), std::ios::binary);
//      std::ofstream ofs(ofile.c_str(), std::ios::binary);
//      for_all_operators_on_disk( *this, *s, ofs, bind(&SparseMatrix::renormalise_transform_on_disk, _1, boost::ref(rotateMatrix), s, boost::ref(ifs)) );
//      ifs.close();
//      ofs.close();
//      int result = rename( ofile.c_str(), ifile.c_str() );
//      assert( result == 0 );
//      // DEBUG only: now read back into core
//      if (false) { 
//        std::ifstream ifs2(ifile.c_str(), std::ios::binary);
//        for_all_operators_multithread( *this, bind(&SparseMatrix::read_from_disk, _1, boost::ref(ifs2)) );
//        ifs2.close();
//      }
//    }
    else {
      // For operators built in core
      for_all_operators_multithread( *this, bind(&SparseMatrix::renormalise_transform, _1, boost::ref(rotateMatrix), s) );
    }
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

  std::vector<boost::shared_ptr<SparseMatrix> > get_local_element(int i) 
  {
    std::vector<boost::shared_ptr<SparseMatrix> > vec(m_op.get_local_element(i).size());
    for (int l=0; l<vec.size(); l++)
      vec[l] = m_op.get_local_element(i)[l]; 
    return vec;
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

  std::vector<boost::shared_ptr<SparseMatrix> > get_global_element(int i)
  {
    std::vector<boost::shared_ptr<SparseMatrix> > vec(m_op.get_global_element(i).size());
    for (int l=0; l<vec.size(); l++)
      vec[l] = m_op.get_global_element(i)[l]; 
    return vec;
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

  std::vector< std::vector<int> > get_array() const 
  {
//    std::vector<int> orbs(2);
    std::vector< std::vector<int> > ret_val(m_op.local_nnz());
    for (int i=0; i<m_op.local_nnz(); i++) {
//MAW >>>>>
//FIXME for 3-index
//      pair<int, int> opair = m_op.unmap_local_index(i);
//      orbs[0] = opair.first; orbs[1] = opair.second;
//      ret_val[i] = orbs;
      ret_val[i] = m_op.unmap_local_index(i);
//MAW <<<<<
    }
    return ret_val;
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

  const std::vector<boost::shared_ptr<SparseMatrix> >  get_element(int i, int j=-1, int k=-1, int l=-1) const 
  {
    std::vector<boost::shared_ptr<SparseMatrix> > vec(m_op(i,j,k,l).size());
    for (int p=0; p<vec.size(); p++)
      vec[p] = m_op(i,j,k,l)[p]; 
    return vec;
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

  std::vector<boost::shared_ptr<SparseMatrix> >  get_element(int i, int j=-1, int k=-1, int l=-1)
  {
    std::vector<boost::shared_ptr<SparseMatrix> > vec(m_op(i,j,k,l).size());
    for (int p=0; p<vec.size(); p++)
      vec[p] = m_op(i,j,k,l)[p]; 
    return vec;
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

  boost::shared_ptr<SparseMatrix> get_op_rep(const SpinQuantum& s, int i=-1, int j=-1, int k=-1, int l=-1)
  {
    assert( k ==-1 );
    assert( l ==-1 );
    Op* o = 0;
    std::vector<boost::shared_ptr<Op> >& vec = m_op(i,j,k,l);
    for (int p=0; p<vec.size(); p++) {
      if ( s == vec[p]->get_deltaQuantum() ) return m_op(i,j,k,l)[p];
    }
    assert (false);
    return boost::shared_ptr<Op>(o);
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

  const boost::shared_ptr<SparseMatrix> get_op_rep(const SpinQuantum& s, int i=-1, int j=-1, int k=-1, int l=-1) const
  {
    assert( k ==-1 );
    assert( l ==-1 );
    Op* o = 0;
    const std::vector<boost::shared_ptr<Op> >& vec = m_op(i,j,k,l);
    for (int p=0; p<vec.size(); p++) {
      if ( s == vec[p]->get_deltaQuantum() ) return m_op(i,j,k,l)[p];
    }
    assert (false);
    return boost::shared_ptr<Op>(o);
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------
// MAW FIXME for more than 2-index operators:
  boost::shared_ptr<SparseMatrix> get_op_rep(const std::vector<SpinQuantum>& s, int i=-1, int j=-1, int k=-1, int l=-1)
  {
    assert( k !=-1 );
    Op* o = 0;
    std::vector<boost::shared_ptr<Op> >& vec = m_op(i,j,k,l);
    assert( vec[0]->get_built() );
    std::string build_pattern = vec[0]->get_build_pattern();
    for (int p=0; p<vec.size(); p++) {
      if ( s == vec[p]->get_quantum_ladder().at(build_pattern) ) return m_op(i,j,k,l)[p];
    }
    assert (false);
    return boost::shared_ptr<Op>(o);
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------
// MAW FIXME for more than 2-index operators:
  const boost::shared_ptr<SparseMatrix> get_op_rep(const std::vector<SpinQuantum>& s, int i=-1, int j=-1, int k=-1, int l=-1) const
  {
    assert( k !=-1 );
    Op* o = 0;
    const std::vector<boost::shared_ptr<Op> >& vec = m_op(i,j,k,l);
    std::string build_pattern = vec[0]->get_build_pattern();
    for (int p=0; p<vec.size(); p++) {
      if ( s == vec[p]->get_quantum_ladder().at(build_pattern) ) return m_op(i,j,k,l)[p];
    }
    assert (false);
    return boost::shared_ptr<Op>(o);
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

};

template <class Op> int Op_component<Op>::nIDgenerator = 1;

//===========================================================================================================================================================
}
#endif
