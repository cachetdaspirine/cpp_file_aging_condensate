#include "../Shared_Objects/Header.h"
#include "Gillespie.h"

using namespace std;

// -----------------------------------------------------------------------------
// -----------------------------accessor----------------------------------------
// -----------------------------------------------------------------------------

int Gillespie::get_N_strand() const { return loop_link.get_strand_size(); }

void Gillespie::get_R(double *R, int size) const
{
  if (size != 3 * (loop_link.get_strand_size()-1))
  {
    throw invalid_argument("invalid size in Gillespie::get_R");
  }
  // We construct an vector of the x,y,z coordinates to sort them according to x
  // for( auto& it : R_to_sort){for(auto& it2 : it){cout<<it2<<" ";}cout<<endl;}
  // fill the R array
  int n(0);
  for (auto &it : loop_link.get_strands())
  {
    if(it->get_Rleft()==nullptr){continue;}
    R[n] = it->get_Rleft()->r()[0];
    R[n + 1] = it->get_Rleft()->r()[1];
    R[n + 2] = it->get_Rleft()->r()[2];
    n += 3;
  }
}

void Gillespie::get_ell_coordinates(double* ell_coordinate,int size)const 
{
  if(size != loop_link.get_strand_size()-1){throw invalid_argument("invalid size in Gillespie::get_R");}
int n(0);
  for(auto& strand : loop_link.get_strands()){
    if(strand->get_Rleft()!=nullptr){
    ell_coordinate[n] = strand->get_ell_coordinate_0();
    n++;
    }
  }
}

void Gillespie::get_ell(double *ells, int size) const
{
  if (size != loop_link.get_strand_size())
  {
    throw invalid_argument("invalid size in Gillespie::get_ell");
  }
  int n(0);
  for (auto &it : loop_link.get_strands())
  {
    ells[n] = it->get_ell();
    n++;
  }
}

void Gillespie::get_r(double *r, int size,bool periodic) const
{
  if (size != get_r_size())
  {
    throw invalid_argument("invalid size in Gillespie::get_r");
  }
  int n(0);
  for (auto &loop : loop_link.get_strands())
  {
    for (auto &link : loop->get_r())
    {
      r[3 * n] = link->r(periodic).at(0);
      r[3 * n + 1] = link->r(periodic).at(1);
      r[3 * n + 2] = link->r(periodic).at(2);
      n++;
    }
  }
}

int Gillespie::get_r_size() const
{
  int size(0);
  for (auto &it : loop_link.get_strands())
  {
    size += it->get_r().size();
  }
  return 3 * size;
}

double Gillespie::get_S() const
{
  double S(0);
  for (auto &it : loop_link.get_strands())
  {
    S +=  it->get_S();
  }
  return S;
}

void Gillespie::get_S_array(double* S, int size) const
{
  if (size != loop_link.get_strand_size())
  {
    throw invalid_argument("invalid size in Gillespie::get_ell");
  }
  int n(0);
  for (auto &it : loop_link.get_strands())
  {
    S[n] = it->get_S();
    n++;
  }
}

double Gillespie::get_F() const
{
  double F((get_N_strand()-1)*binding_energy);
  for (auto &it : loop_link.get_strands())
  {
    F += - it->get_S();
  }
  return F;
}

void Gillespie::Print_Loop_positions() const
{
  for (auto &loop : loop_link.get_strands())
  {
    //cout << "theta Phi " << loop->get_theta() << " " << loop->get_phi() << endl;
    //cout << "xg,yg,zg " << loop->get_Rg()[0] << " " << loop->get_Rg()[1] << " " << loop->get_Rg()[2] << endl;
    cout << "volume " << loop->get_V() << endl;
    cout << "ell " << loop->get_ell() << endl;
    cout<<"anchoring points : ";
    if(loop->get_Rleft()!=nullptr){cout << " left :" << loop->get_Rleft()->r()[0] << " " << loop->get_Rleft()->r()[1] << " " << loop->get_Rleft()->r()[2] ;}
    if(loop->get_Rright()!=nullptr){cout<< " right : " << loop->get_Rright()->r()[0] << " " << loop->get_Rright()->r()[1] << " " << loop->get_Rright()->r()[2] ;}
    cout<< endl;
    cout << "number of crosslinkers of this loop :" << loop->get_r().size() << endl
         << endl;
  }
  for(auto& key_linker : loop_link.get_linkers())
  {
    cout<<key_linker.second->is_free()<<" "<<key_linker.second->r()[0]<<" "<<key_linker.second->r()[1]<<" "<<key_linker.second->r()[2]<<endl;
  }
}

void Gillespie::check_loops_integrity()
{
  IF(true){cout<<"Gillespie : Start checking loops integrity"<<endl;}
  // check that the occupied linkers are in strand occupied container
  for(auto& loop: loop_link.get_strands())
  {
    for(auto& linker : loop->get_r())
    {
      if(linker->is_free()){}
      else
      {
        cout<<"linker with an issue : "<<linker<<endl;
        cout<<"loop with an issue : "<<loop<<endl;
        throw invalid_argument("an occupied linker is in a free container");
      }
    }
    for(auto& linker : loop->get_occ_r())
    {
      if(linker->is_free())
      {
        cout<<"linker with an issue : "<<linker<<endl;
        cout<<"loop with an issue : "<<loop<<endl;
        throw invalid_argument("a free linker is in an occupied container");
      }
    }
  }
  // check that the linker of every strand own the given strand
  for(auto& loop:loop_link.get_strands())
  {
    //cout<<loop<<endl;
    for(auto& linker : loop->get_r())
    {
      if(linker->get_strands().find(loop)==linker->get_strands().end())
      {
        cout<<"loop issue : "<<loop<<endl;
        cout<<"linker issue : "<<linker<<endl;
        throw invalid_argument("a loop own a linker that doesn't own this loop");
      }
    }
    for(auto& linker : loop->get_occ_r())
    {
      if(linker->get_strands().find(loop)==linker->get_strands().end())
      {
        cout<<"loop issue : "<<loop<<endl;
        cout<<"linker issue : "<<linker<<endl;
        throw invalid_argument("a loop own a linker that doesn't own this loop");
      }
    }
  }
  IF(true){cout<<"Strands and linkers are consistent"<<endl;}
  // ccheck that the strand of every linkers contain the given linker
  //cout<<"linkers address :"<<endl;
  /*
  ofstream out_linker;
  out_linker.open("linkers.txt",ios::out);
  for(auto& slice1:loop_link.get_linkers())
  {
    for(auto& slice2 : slice1.second)
    {
      for(auto& linker : slice2.second)
      {
        out_linker<<linker.second<<endl;
        for(auto& loop:linker.second->get_strands())
        {
          if(find(loop->get_r().begin(),loop->get_r().end(),linker.second)==loop->get_r().end()
            && find(loop->get_occ_r().begin(),loop->get_occ_r().end(),linker.second)==loop->get_occ_r().end())
          {
            cout<<"loop issue : "<<loop<<endl;
            cout<<"linker issue :"<<linker.second<<endl;
            throw invalid_argument("a linker own a loop that does not own this linker");
          }
        }
      }
    }
  }
  out_linker.close();
  */
}

void Gillespie::get_r_gillespie(double *r, int size,bool periodic)const
{
  if (size != get_r_gillespie_size())
  {
    throw invalid_argument("invalid size in Gillespie::get_r");
  }
  //int n(0);
  for(auto& it : loop_link.get_linkers())
  {
        //r[3*n]    =   it.second->r()[0];
        //r[3*n+1]  =   it.second->r()[1];
        //r[3*n+2]  =   it.second->r()[2];
        r[3*it.second->g_ID()]    =   it.second->r(periodic)[0];
        r[3*it.second->g_ID()+  1]  =   it.second->r(periodic)[1];
        r[3*it.second->g_ID()+2]  =   it.second->r(periodic)[2];
        //n++;
  }
}

int Gillespie::get_r_gillespie_size() const
{
  return 3*loop_link.get_linker_size();
}

void Gillespie::print_random_stuff() const{
  //linkers.print();
  cout<<"loops linkers : "<<endl;
  for(auto& it : loop_link.get_strands()){for(auto& link : it->get_r()){
    cout<<link->r()[0]<< " " << link->r()[1] << " "<<link->r()[2]<<endl;
  }}
}
