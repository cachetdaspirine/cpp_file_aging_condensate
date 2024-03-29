#ifndef LoopLinkWrap_h
#define LoopLinkWrap_h
//class LoopLinkWrap;
class LoopLinkWrap
{
    private:        
        std::map<std::array<double,3>,Linker*> linkers;
        std::set<Strand*,LessLoop> strands;
        int Nfree_linker;
        void set_p_linkers(Strand* newly_created_strand);        
        void get_in_ellipse(   const  std::array<double,3>& ctr_mass,
                                const std::array<double,3>& main_ax,
                                double a,
                                double b,
                                std::vector<Linker*>& free_linkers,
                                std::vector<Linker*>& occ_linkers) const;
        int dimension;
    public:        
        LoopLinkWrap(int dim);
        ~LoopLinkWrap();
        int g_dim() const;
        /*
            (~_|_ _ _  _  _| _
            _) | | (_|| |(_|_\                                                              
        */
        void reset_strands(std::set<Strand*,LessLoop> new_strands);

        Strand* Create_Strand(const Strand& new_strand);

        void Remove_Strand(Strand* strand_to_remove);
        
        void delete_strands();

        std::set<Strand*,LessLoop>::iterator get_strand(int distance);

        std::set<Strand*,LessLoop> const& get_strands() const;

        int get_strand_size() const;
        

       /*
            | . _ |  _  _ _
            |_|| ||<(/_| _\                    
       */
                
        //void add_linker(Linker* to_add);

        void set_free(Linker* link);

        void set_occupied(Linker* link);

        int get_N_free_linker() const;

        void create_new_free_linker(double x,double y, double z);
        
        void create_new_occupied_linker(double x,double y,double z);
                
        void delete_linkers();
        void delete_free_linkers();
        
        int get_linker_size() const;
        
        std::map<std::array<double,3>,Linker*>const &  get_linkers() const;
        
        std::map<std::array<double,3>,Linker*> get_linkers3d() const;

        Linker* diffuse_linker(std::array<double,3> r,Linker* linker);
        Linker* get_random_free_linker();
        
        /*
        |\/|. _ _ _ || _  _  _  _     _
        |  ||_\(_(/_||(_|| |(/_(_)|_|_\
        */


        void remake_strands(std::set<Strand*,LessLoop> to_remake);

        void delete_pointers();

        int counter;
};
class Accessor
{
    private:
        static void compute_rates(Strand* strand_to_compute_rates);
        static Strand* clone(const Strand& strand_to_clone);
    friend Strand* LoopLinkWrap::Create_Strand(const Strand& new_strand);
};
#endif