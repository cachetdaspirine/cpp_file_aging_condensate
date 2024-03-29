#include "Header.h"

using namespace std;

void Accessor::compute_rates(Strand* strand_to_compute_rates)
{
    strand_to_compute_rates->compute_total_rates();
}

Strand* Accessor::clone(const Strand& strand_to_clone)
{
    return strand_to_clone.clone();
}
LoopLinkWrap::LoopLinkWrap(int dim):dimension(dim){Nfree_linker=0.;counter=0;}
int LoopLinkWrap::g_dim() const{return dimension;}
LoopLinkWrap::~LoopLinkWrap()
{
    //delete_pointers();
}

void LoopLinkWrap::set_p_linkers(Strand* newly_created_strand)
{
  IF(true){cout<<"set_p_linkers"<<endl;}
  // get the volume limit
  array<double,3> main_ax,ctr_mass;
  double a,b;
  newly_created_strand->get_volume_limit(main_ax,ctr_mass,a,b);
  // select the linkers in the vicinity
  std::vector<Linker*> free_linkers;
  free_linkers.reserve(get_N_free_linker());
  std::vector<Linker*> occ_linkers;
  occ_linkers.reserve(get_linker_size()-get_N_free_linker());
  IF(true){cout<<"Strand : slice"<<endl;}
  get_in_ellipse(ctr_mass,main_ax,a,b,free_linkers,occ_linkers);
  // tell the linkers that this strand is around
  for(auto& linker : free_linkers){linker->add_strand(newly_created_strand);}
  for(auto& linker : occ_linkers){linker->add_strand(newly_created_strand);}
  newly_created_strand->set_linkers(free_linkers,occ_linkers);
}

/*
    (~_|_ _ _  _  _| _
    _) | | (_|| |(_|_\                                                              
*/

void LoopLinkWrap::reset_strands(set<Strand*,LessLoop> new_strands)
{
    strands = new_strands;
}

Strand* LoopLinkWrap::Create_Strand(const Strand& new_strand)
{
    //Strand* new_created_strand = new_strand.clone();
    Strand* new_created_strand = Accessor::clone(new_strand);
    strands.insert(new_created_strand);
    set_p_linkers(new_created_strand);
    //new_created_strand->compute_all_rates();
    Accessor::compute_rates(new_created_strand);
    return new_created_strand;
}

void LoopLinkWrap::Remove_Strand(Strand* strand_to_remove)
{
    IF(true){cout<<"LoopLinkWrapper : start removing a strand"<<endl;}
    strands.erase(strand_to_remove);
    strand_to_remove->remove_from_linkers();
    delete strand_to_remove;
}

set<Strand*,LessLoop>::iterator LoopLinkWrap::get_strand(int distance)
{return next(strands.begin(),distance);}

void LoopLinkWrap::delete_strands()
{for(auto& strand : strands){delete strand;}strands.clear();}

int LoopLinkWrap::get_strand_size() const {return strands.size();}

set<Strand*,LessLoop> const & LoopLinkWrap::get_strands() const {return strands;}


/*
    | . _ |  _  _ _
    |_|| ||<(/_| _\                    
*/

void LoopLinkWrap::set_free(Linker* link)
{
    Nfree_linker++;
    link->set_free();
}

void LoopLinkWrap::set_occupied(Linker* link)
{
    Nfree_linker--;
    link->set_bounded();
}

void LoopLinkWrap::create_new_free_linker(double x,double y, double z)
{
    linkers[{x,y,z}] = new Linker({x,y,z},dimension,counter);
    counter++;
    Nfree_linker++;    
}

int LoopLinkWrap::get_N_free_linker() const
{
    return Nfree_linker;
}

void LoopLinkWrap::create_new_occupied_linker(double x, double y, double z)
{
    Linker* link = new Linker({x,y,z},dimension,counter);
    counter++;
    link->set_bounded();
    linkers[{x,y,z}] = link;
}

void LoopLinkWrap::delete_linkers()
{
    // Simply delete all the pointer in linkers and initialize a new empty map
    IF(true){cout<<"LoopLinkWrap : delete the linkers pointer"<<endl;}
    // doesn t delete them from the strands objects
    Nfree_linker = 0.;
    for(auto& linker : linkers){
        delete linker.second;
        counter--;
    }
    map<array<double,3>,Linker*> newlinkers; // make a new empty map3d
    linkers = newlinkers;
}
void LoopLinkWrap::delete_free_linkers()
{
    // Simply delete all the free linkers pointers
    IF(true){cout<<"LoopLinkWrap : delete the free linkers pointer"<<endl;}
    // doesn t delete them from the strands objects
    for(auto linker = linkers.begin(); linker!=linkers.end();)
    {
        if((*linker).second->is_free()){
            delete (*linker).second;
            counter--;
            linker = linkers.erase(linker);
            Nfree_linker--;
        }
        else{
            ++linker;
        }
    }
    //map<array<double,3>,Linker*> newlinkers; // make a new empty map3d
    //linkers = newlinkers; 
}

map<array<double,3>,Linker*> const &LoopLinkWrap::get_linkers() const
{
    return linkers;
}

Linker* LoopLinkWrap::get_random_free_linker()
{
    int step(0);
    while(step<pow(10.,17.))
    {
        auto item(linkers.begin());
        std::uniform_int_distribution<int> distribution(0,linkers.size()-1);
        std::advance(item, distribution(generator));
        if((*item).second->is_free())
        {
            return (*item).second;
        }
    }
    std::cout<<"no free linker to draw Nfree_linker certainly wrong"<<std::endl;
    throw std::out_of_range("Nfree_wrong ?");
}

int LoopLinkWrap::get_linker_size() const{
    return linkers.size();
}

//void LoopLinkWrap::add_linker(Linker* to_add){
//    linkers.add(to_add->r()[0],
//                to_add->r()[1],
//                to_add->r()[2],
//                to_add);
//}
Linker* LoopLinkWrap::diffuse_linker(array<double,3> r,Linker* linker){
    linkers.erase({linker->r()[0],linker->r()[1],linker->r()[2]});
    //cout<<"diffuse"<<endl;
    linker->diffuse(r);
    //cout<<"diffused"<<endl;
    linkers[{linker->r()[0],linker->r()[1],linker->r()[2]}] = linker;
    return linker;
}
/*
    |\/|. _ _ _ || _  _  _  _     _
    |  ||_\(_(/_||(_|| |(/_(_)|_|_\
*/
void LoopLinkWrap::delete_pointers()
{
    IF(true){cout<<"delete_pointers"<<endl;}
    delete_strands();
    delete_linkers();
}

/*
void LoopLinkWrap::get_in_ellipse( const std::array<double,3>& ctr_mass,
                                    const std::array<double,3>& main_ax,
                                    double a,
                                    double b,
                                    std::vector<Linker*>& free_linkers,
                                    std::vector<Linker*>& occ_linkers)const
{
    double theta = atan2(main_ax[1],main_ax[0]);
    array<array<double,3>,3> OmZ(OmegaZ(-theta));
    double phi = atan2(sqrt(pow(main_ax[0],2)+pow(main_ax[1],2)),main_ax[2])-acos(-1)/2;
    array<array<double,3>,3> OmY(OmegaY(-phi));

    
    array<double,3> r;
    for(auto& linker: linkers)
    {  
        r = dot(OmY,dot(OmZ,Minus(linker.second->r(),ctr_mass)));
        if(pow(r[0]/a,2)+pow(r[1]/b,2)+pow(r[2]/b,2) <= 1)
        {
            if(linker.second->is_free()){
                free_linkers.push_back(linker.second);
                }
            else{
                occ_linkers.push_back(linker.second);
                }
        }
    }
}
*/

void LoopLinkWrap::get_in_ellipse(const std::array<double,3>& ctr_mass,
                        const std::array<double,3>& main_ax,
                        double a, double b,
                        std::vector<Linker*>& free_linkers,
                        std::vector<Linker*>& occ_linkers) const
    {
        const double pi = 3.141592653589793;
        double theta = atan2(main_ax[1], main_ax[0]);
        auto OmZ = OmegaZ(-theta);
        double phi = atan2(sqrt(main_ax[0] * main_ax[0] + main_ax[1] * main_ax[1]), main_ax[2]) - pi / 2;
        auto OmY = OmegaY(-phi);
        auto Om_tot (dot(OmY,OmZ));
        // Precompute squares of semi-axes to use in comparison
        double a_squared = a * a;
        double b_squared = b * b;

        for (auto& linker : linkers) { // Assuming 'linkers' is accessible
            //auto r = dot(OmY, dot(OmZ, Minus(linker.second->r(), ctr_mass)));
            auto r(dot(Om_tot,Minus(linker.second->r(), ctr_mass)));
            // Compare squares to avoid sqrt; no need for pow for squaring
            if (r[0] * r[0] / a_squared + r[1] * r[1] / b_squared + r[2] * r[2] / b_squared <= 1) {
                if (linker.second->is_free()) {
                    free_linkers.push_back(linker.second);
                } else {
                    occ_linkers.push_back(linker.second);
                }
            }
        }
    }
    // Definitions of OmegaZ, OmegaY, dot, and Minus would be here or elsewhere
//};

void LoopLinkWrap::remake_strands(set<Strand*,LessLoop> to_remake)
{
    // this function just remake the list of strands given.
    // Recreating has only one function : recomputing the rates.
    IF(true){cout<<"LoopLinkWrap : actualize_vicinity"<<endl;}
    for(auto& strand : to_remake)
    {
        Create_Strand(*strand);
    }
    for(auto& strand : to_remake){Remove_Strand(strand);}
    IF(true){cout<<"LoopLinkWrap : finished actualizing vicinity"<<endl;}
}