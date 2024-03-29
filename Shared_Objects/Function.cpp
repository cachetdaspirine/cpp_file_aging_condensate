#include "Header.h"
using namespace std;
double Pi = acos(-1);
//default_random_engine generator;
mt19937 generator;
array<double,3> anchor;
//double get_square_diff(array<double,3> v1,array<double,3> v2){
//  double res(0);
//  for(int n=0;n<3;n++){
//    res+=pow(v1[n]-v2[n],2);
//  }
//  return res;
//}
//double diff(array<double,3> v1,array<double,3> v2){
//  double res(0);
//  for(int n=0;n<3;n++){
//    res+=pow(v1[n]-v2[n],2);
//  }
//  return sqrt(res);
//}
//array<double,3> dot(array<array<double,3>,3> Matrice,array<double,3> vect)
//{
//  array<double,3> res({0.,0.,0.});
//  for(int i =0; i<3;i++)
//  {
//    for(int j=0;j<3;j++)
//    {
//      res[i] += Matrice[i][j] * vect[j];
//    }
//  }
//  return res;
//}
array<array<double,3>,3> OmegaY(double theta)
{
  array<array<double,3>,3> res;
  for(int i =0;i<3;i++){res[i] = {0.,0.,0.};}
  res[0][0] = cos(theta);
  res[0][2] = sin(theta);
  res[1][1] = 1.;
  res[2][0] = -sin(theta);
  res[2][2] = cos(theta);
  return res;
}
array<array<double,3>,3> OmegaZ(double theta)
{
  array<array<double,3>,3> res;
  for(int i =0;i<3;i++){res[i] = {0.,0.,0.};}
  res[0][0] = cos(theta);
  res[0][1] = -sin(theta);
  res[2][2] = 1.;
  res[1][0] = sin(theta);
  res[1][1] = cos(theta);
  return res;
}
double norm(array<double,3> u)
{
  return sqrt(pow(u[0],2)+pow(u[1],2)+pow(u[2],2));
}
array<array<double,3>,3> ax_from_main_ax(array<double,3> u,double a,double b)
{
  // constructe two perpendicular vector
  array<double,3> v,w;
  if(u[2] != 0)
  {
    v = {1.,1.,(-u[0]-u[1])/u[2]};
    w = {u[1]*(-u[0]-u[1])/u[2]-u[2], 
                  u[2]-u[0]*(-u[0]-u[1])/u[2],
                 u[0]-u[1]};
  }
  else
  {
    v = {0.,0.,1.};
    w ={u[1],u[0],0};
  }
  //normalize them
  double nu(norm(u)),nv(norm(v)),nw(norm(w));
  for(int i = 0; i<3;i++){
    u[i] =a*u[i] / nu;
    v[i] =b*v[i] / nv;
    w[i] =b*w[i] / nw;
  }
  return {u,v,w};
}

void generate_point_in_ellipse( array<double,3> main_ax,
                                array<double,3> ctr_mass,
                                double a, 
                                double b,
                                set<array<double,3>>& res,
                                int N_linker)
{
  double n(norm(main_ax));
  if(n!=0){
    for(int i=0;i<3;i++){
      main_ax[i] = main_ax[i]/n;}
  }
  array<array<double,3>,3> axes(ax_from_main_ax(main_ax,a,b));

  double theta = atan2(main_ax[1],main_ax[0]);
  
  array<array<double,3>,3> OmZ(OmegaZ(theta));
  
  double phi = atan2(sqrt(pow(main_ax[0],2)+pow(main_ax[1],2)),main_ax[2])-acos(-1)/2;

  array<array<double,3>,3> OmY(OmegaY(phi));
  uniform_real_distribution<double> Phis(0.,2.*acos(-1.));
  uniform_real_distribution<double> CosTheta(-1.,1.);
  uniform_real_distribution<double> R(0.,1.);

  double x,y,z,r;
  for(int i = 0; i<N_linker;i++)
  {
    phi = Phis(generator);
    theta = acos(CosTheta(generator));
    r = pow(R(generator),1./3.);

    x = r*sin(theta)*cos(phi);
    y = r*sin(theta)*sin(phi);
    z = r*cos(theta);

    res.insert(Plus(ctr_mass,dot(OmZ,dot(OmY,dot(axes,std::array<double, 3>{x,y,z})))));
  }
  //ofstream file;
  //file.open("trash.txt",std::ios::app);
  //for(auto& pts : res){file<<pts[0]<<" "<<pts[1]<<" "<<pts[2]<<endl;}
}
/*bool is_point_in_ellipsoid(array<double,3> main_ax, array<double,3> ctr_mass, double a, double b, array<double,3> point) {
    // Translate the point to the ellipsoid's center
    array<double,3> translated_point = {point[0] - ctr_mass[0], point[1] - ctr_mass[1], point[2] - ctr_mass[2]};
    
    // Assuming ax_from_main_ax, OmegaZ, and OmegaY functions are defined and compute the necessary transformations
    array<array<double,3>,3> axes = ax_from_main_ax(main_ax, a, b); // Calculate axes based on the main axis and ellipsoid dimensions

    // Transform the point to the ellipsoid's local coordinates
    array<double,3> local_point = dot(axes, translated_point);

    // Check if the point is inside the ellipsoid
    return (pow(local_point[0], 2) / pow(a, 2) + pow(local_point[1], 2) / pow(b, 2) + pow(local_point[2], 2) / pow(b, 2)) <= 1;
}*/
bool is_point_in_ellipsoid(const std::array<double, 3>& main_ax, 
                            const std::array<double, 3>& ctr,
                           double a, double b, 
                           const std::array<double, 3>& point) {
    // Normalize the main axis
    double n = norm(main_ax);
    std::array<double, 3> norm_main_ax = {main_ax[0] / n, main_ax[1] / n, main_ax[2] / n};
    
    // Compute theta and phi from the main axis orientation
    double theta = atan2(norm_main_ax[1], norm_main_ax[0]);
    double phi = atan2(sqrt(pow(norm_main_ax[0], 2) + pow(norm_main_ax[1], 2)), norm_main_ax[2]) - acos(-1) / 2;
    
    // Inverse rotation matrices to align point with canonical ellipsoid axes
    auto OmZ_inv = OmegaZ(-theta);  // Inverse rotation around Z-axis
    auto OmY_inv = OmegaY(-phi);    // Inverse rotation around Y-axis
    
    // Transform the point to the ellipsoid's local coordinate system
    auto p_minus_ctr = Minus(point, ctr);
    auto p_local = dot(OmY_inv, dot(OmZ_inv, p_minus_ctr));
    
    // Check if the point is inside the ellipsoid using its canonical equation
    double x = p_local[0], y = p_local[1], z = p_local[2];
    return (pow(x, 2) / pow(a, 2)) + (pow(y, 2) / pow(b, 2)) + (pow(z, 2) / pow(a, 2)) <= 1;
}

void sph2cart(double r, double theta, double phi, double& x, double& y, double& z)
{
    x = r * sin(theta) * cos(phi);
    y = r * sin(theta) * sin(phi);
    z = r * cos(theta);
}

// Convert Cartesian coordinates to spherical coordinates
void cart2sph(double x, double y, double z, double& r, double& theta, double& phi)
{
    r = sqrt(x*x + y*y + z*z);
    theta = acos(z / r);
    phi = atan2(y, x);
}