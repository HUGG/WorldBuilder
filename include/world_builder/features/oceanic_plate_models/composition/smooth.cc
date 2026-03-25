#include "world_builder/features/oceanic_plate_models/composition/smooth.h"

#include <world_builder/assert.h>
#include <world_builder/nan.h>
#include <world_builder/parameters.h>
#include <world_builder/utilities.h>

#include <world_builder/types/array.h>
#include <world_builder/types/double.h>
#include <world_builder/types/object.h>
#include <world_builder/types/one_of.h>
#include <world_builder/types/string.h>
#include <world_builder/types/unsigned_int.h>
#include <world_builder/types/value_at_points.h>


namespace WorldBuilder
{
  using namespace Utilities;

  namespace Features
  {
    namespace OceanicPlateModels
    {
      namespace Composition
      {
        Smooth::Smooth(WorldBuilder::World *world_)
          :
          min_depth(NaN::DSNAN),
          max_depth(NaN::DSNAN),
          operation(Operations::REPLACE)
        {
          this->world = world_;
          this->name = "smooth";
        }

        Smooth::~Smooth()
          = default;

        void
        Smooth::declare_entries(Parameters &prm, const std::string & /*unused*/)
        {
          // Document plugin and require entries if needed.
          // Add compositions to the required parameters.
          prm.declare_entry("", Types::Object({"compositions"}),
                            "Smooth compositional model. Sets linearly varying gradient between two depths for a compositional field.");

          // Declare entries of this plugin.
          prm.declare_entry("min depth", Types::OneOf(Types::Double(0),
                                                      Types::Array(Types::ValueAtPoints(0., 2)),
                                                      Types::String("")),
                            "The depth in meters from which the composition of this feature is present.");

          prm.declare_entry("max depth", Types::OneOf(Types::Double(std::numeric_limits<double>::max()),
                                                      Types::Array(Types::ValueAtPoints(std::numeric_limits<double>::max(), 2)),
                                                      Types::String("")),
                            "The depth in meters to which the composition of this feature is present.");
          prm.declare_entry("top fractions", Types::Array(Types::Double(1.0), 1),
                            "The composition fraction on the top boundary of the layer.");
          prm.declare_entry("bottom fractions", Types::Array(Types::Double(0.0), 1),
                            "The composition fraction on the bottom boundary of the layer.");  
          prm.declare_entry("compositions", Types::Array(Types::UnsignedInt(), 0),
                            "A list with the labels of the composition which are present there.");
          prm.declare_entry("operation", Types::String("replace", std::vector<std::string> {"replace", "replace defined only", "add", "subtract"}),
                            "Whether the value should replace any value previously defined at this location (replace) or "
                            "add the value to the previously define value. Replacing implies that all compositions not "
                            "explicitly defined are set to zero. To only replace the defined compositions use the replace only defined option.");
        }

        void
        Smooth::parse_entries(Parameters &prm, const std::vector<Point<2>> &coordinates)
        {
          min_depth_surface = Objects::Surface(prm.get("min depth", coordinates));
          min_depth = min_depth_surface.minimum;
          max_depth_surface = Objects::Surface(prm.get("max depth", coordinates));
          max_depth = max_depth_surface.maximum;
          top_fraction = prm.get_vector<double>("top fractions");
          bottom_fraction = prm.get_vector<double>("bottom fractions");
          compositions = prm.get_vector<unsigned int>("compositions");
          operation = string_operations_to_enum(prm.get<std::string>("operation"));
        }


        double
        Smooth::get_composition(const Point<3> & /*position_in_cartesian_coordinates*/,
                                const Objects::NaturalCoordinate &position_in_natural_coordinates,
                                const double depth,
                                const unsigned int composition_number,
                                double composition_,
                                const double /*feature_min_depth*/,
                                const double /*feature_max_depth*/) const
        {
          double composition = composition_;

          if (depth <= max_depth && depth >= min_depth)
            {
              const double min_depth_local = min_depth_surface.constant_value ? min_depth : min_depth_surface.local_value(position_in_natural_coordinates.get_surface_point()).interpolated_value;
              const double max_depth_local = max_depth_surface.constant_value ? max_depth : max_depth_surface.local_value(position_in_natural_coordinates.get_surface_point()).interpolated_value;
              if (depth <= max_depth_local && depth >= min_depth_local)
                {
                  for (unsigned int i = 0; i < compositions.size(); i++)
                    {
                      if (compositions[i] == composition_number)
                        {
                          composition = (top_fraction[i] - bottom_fraction[i]) * (min_depth_local - depth) / (max_depth_local - min_depth_local) + top_fraction[i];
                          return apply_operation(operation, composition_, composition);
                        }
                    } // Composition loop  
                } // Check bounds for local depths   
            } // Check bounds for global depths
          return composition;
        } // function get_composition
        WB_REGISTER_FEATURE_OCEANIC_PLATE_COMPOSITION_MODEL(Smooth, smooth)
      } // namespace Composition
    } // namespace OceanicPlateModels
  } // namespace Features
} // namespace WorldBuilder