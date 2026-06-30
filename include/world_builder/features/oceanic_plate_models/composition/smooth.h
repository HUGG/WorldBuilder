#ifndef WORLD_BUILDER_FEATURES_OCEANIC_PLATE_MODELS_COMPOSITION_SMOOTH_H
#define WORLD_BUILDER_FEATURES_OCEANIC_PLATE_MODELS_COMPOSITION_SMOOTH_H

#include <world_builder/features/oceanic_plate_models/composition/interface.h>
#include <world_builder/features/feature_utilities.h>
#include <world_builder/objects/surface.h>


namespace WorldBuilder
{
  namespace Features
  {
    using namespace FeatureUtilities;
    namespace OceanicPlateModels
    {
      namespace Composition
      {
        /**
         * This class represents a oceanic plate and can implement submodules
         * for temperature and composition. These submodules determine what
         * the returned temperature or composition of the temperature and composition
         * functions of this class will be.
         */
        class Smooth: public Interface 
        {
          public:
            /**
             * constructor
             */
            Smooth(WorldBuilder::World *world);

            /**
             * Destructor
             */
            ~Smooth();

            /**
             * declare and read in the world builder file into the parameters class
             */
            static
            void declare_entries(Parameters &prm, const std::string &parent_name = "");

            /**
             * declare and read in the world builder file into the parameters class
             */
            void parse_entries(Parameters &prm, const std::vector<Point<2>> &coordinates) override final;


            /**
             * Returns a composition based on the given position, depth in the model
             * and current composition.
             */
            double get_composition(const Point<3> &position,
                                   const Objects::NaturalCoordinate &position_in_natural_coordinates,
                                   const double depth,
                                   const unsigned int composition_number,
                                   double composition,
                                   const double feature_min_depth,
                                   const double feature_max_depth) const override final;
          
          private:
            // smooth composition submodule parameters
            double min_depth;
            Objects::Surface min_depth_surface;
            double max_depth;
            Objects::Surface max_depth_surface;
            std::vector<double> top_fraction;
            std::vector<double> bottom_fraction;
            std::vector<unsigned int> compositions;
            // std::vector<double> fractions;
            Operations operation;
        };
      } // namespace Composition
    } // namespace OceanicPlateModels
  } // namespace Features
} // namespace WorldBuilder

#endif