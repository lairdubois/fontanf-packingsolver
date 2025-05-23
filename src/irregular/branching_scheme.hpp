#pragma once

#include "packingsolver/irregular/solution.hpp"

#include "shape_simplification.hpp"

#include "shape/trapezoid.hpp"

#include "optimizationtools/utils/utils.hpp"

#include <sstream>

namespace packingsolver
{
namespace irregular
{

using ItemShapeTrapezoidPos = int64_t;
using TrapezoidSetId = int64_t;

struct TrapezoidSet
{
    /** Item type. */
    ItemTypeId item_type_id;

    /** Angle. */
    Angle angle;

    /** Mirror. */
    bool mirror;

    /** Shapes. */
    std::vector<std::vector<GeneralizedTrapezoid>> shapes;

    /** Inflated shapes. */
    std::vector<std::vector<GeneralizedTrapezoid>> shapes_inflated;

    /** Supporting parts of the trapezoid set. */
    std::vector<ShapePos> supporting_parts;

    /** Supported parts of the trapezoid set. */
    std::vector<ShapePos> supported_parts;

    LengthDbl x_min;

    LengthDbl x_max;

    LengthDbl y_min;

    LengthDbl y_max;

    void write_svg(
            const std::string& file_path) const;
};

/**
 * Branching scheme class for problem of type "irregular".
 */
class BranchingScheme
{

public:

    /*
     * Sub-structures
     */

    enum class Direction
    {
        LeftToRightThenBottomToTop,
        LeftToRightThenTopToBottom,
        RightToLeftThenBottomToTop,
        RightToLeftThenTopToBottom,
        BottomToTopThenLeftToRight,
        BottomToTopThenRightToLeft,
        TopToBottomThenLeftToRight,
        TopToBottomThenRightToLeft,
        Any,
    };

    /**
     * Structure that stores a point of the skyline.
     */
    struct UncoveredTrapezoid
    {
        UncoveredTrapezoid(
                const GeneralizedTrapezoid& trapezoid):
            trapezoid(trapezoid) { }

        UncoveredTrapezoid(
                ItemTypeId item_type_id,
                ItemShapePos item_shape_pos,
                ItemShapeTrapezoidPos item_shape_trapezoid_pos,
                const GeneralizedTrapezoid& trapezoid):
            item_type_id(item_type_id),
            item_shape_pos(item_shape_pos),
            item_shape_trapezoid_pos(item_shape_trapezoid_pos),
            trapezoid(trapezoid) { }

        UncoveredTrapezoid(
                DefectId defect_id,
                const GeneralizedTrapezoid& trapezoid):
            defect_id(defect_id),
            trapezoid(trapezoid) { }

        /** Item type. */
        ItemTypeId item_type_id = -1;

        /** Item shape. */
        ItemShapePos item_shape_pos = -1;

        /** Item shape rectangle. */
        ItemShapeTrapezoidPos item_shape_trapezoid_pos = -1;

        /** Defect id. */
        DefectId defect_id = -1;

        /** Trapezoid. */
        GeneralizedTrapezoid trapezoid;

        bool operator==(const UncoveredTrapezoid& uncovered_trapezoid) const;
    };

    /**
     * Bin type structure used in the branching scheme.
     */
    struct BranchingSchemeBinType
    {
        /** Trapezoids of the defects. */
        std::vector<UncoveredTrapezoid> defects;

        /** Supporting parts of the bin (includes supporting parts of defects). */
        std::vector<ShapePos> supporting_parts;

        LengthDbl x_min;
        LengthDbl x_max;
        LengthDbl y_min;
        LengthDbl y_max;
    };

    struct Support
    {
        Shape shape;

        TrapezoidSetId trapezoid_set_id = -1;
        ItemShapePos item_shape_pos = -1;
        ShapePos hole_pos = -1;
        BinTypeId bin_type_id = -1;
        DefectId defect_id = -1;

        Point min = {0, 0};
        Point max = {0, 0};
    };

    struct DirectionData
    {
        /** Bin types in each direction. */
        std::vector<BranchingSchemeBinType> bin_types;

        /** Trapezoid sets in each direction. */
        std::vector<TrapezoidSet> trapezoid_sets;

        /*
         * Supports
         */

        /** Supporting parts. */
        std::vector<Support> supporting_parts;

        /** Supported parts. */
        std::vector<Support> supported_parts;
    };

    struct Insertion
    {
        /**
         * - -1: the item is inserted in the last bin
         * - Otherwise, direction
         */
        Direction new_bin_direction = Direction::Any;

        /** Id of the inserted rectangle set. */
        TrapezoidSetId trapezoid_set_id = -1;

        /** Supporting part. */
        ShapePos supporting_part_pos = -1;

        /** Supporting part element. */
        ElementPos supporting_part_element_pos = -1;

        /** x-coordinate of the supporting part. */
        LengthDbl supporting_part_x = 0.0;

        /** y-coordinate of the supporting part. */
        LengthDbl supporting_part_y = 0.0;

        /** Supported part. */
        ShapePos supported_part_pos = -1;

        /** Supported part element. */
        ElementPos supported_part_element_pos = -1;

        /** x-coordinate of the point of interest. */
        LengthDbl x = 0.0;

        /** y-coordinate of the point of interest. */
        LengthDbl y = 0.0;

        LengthDbl ys = 0.0;

        LengthDbl ye = 0.0;

        bool operator==(const Insertion& insertion) const;
        bool operator!=(const Insertion& insertion) const { return !(*this == insertion); }
    };

    /**
     * Node structure of the branching scheme.
     */
    struct Node
    {
        /** Id of the node. */
        NodeId id = -1;

        /**
         * Pointer to the parent of the node,
         * 'nullptr' if the node is the root.
         */
        std::shared_ptr<Node> parent = nullptr;

        /** Last inserted rectangle set. */
        TrapezoidSetId trapezoid_set_id = -1;

        /** x-coordinates of the bottom-left corner of the last inserted item. */
        LengthDbl x = 0.0;

        /** y-coordinates of the bottom-left corner of the last inserted item. */
        LengthDbl y = 0.0;

        LengthDbl ys = 0.0;

        LengthDbl ye = 0.0;

        /** Last bin direction. */
        Direction last_bin_direction = Direction::LeftToRightThenBottomToTop;

        /** Uncovered rectangles. */
        std::vector<UncoveredTrapezoid> uncovered_trapezoids;

        /** Extra rectangles. */
        std::vector<UncoveredTrapezoid> extra_trapezoids;

        /**
         * Skyline of all inserted item trapezoids used to compute the guide
         * area.
         */
        std::vector<UncoveredTrapezoid> all_trapezoids_skyline;

        /** For each item type, number of copies in the node. */
        std::vector<ItemPos> item_number_of_copies;

        /** Number of bins in the node. */
        BinPos number_of_bins = 0;

        /** Number of items in the node. */
        ItemPos number_of_items = 0;

        /** Item area. */
        AreaDbl item_area = 0;

        /** Sum of the area of the convex hull of each packed item. */
        AreaDbl item_convex_hull_area = 0;

        /** Leftover value. */
        Profit leftover_value = 0;

        /** Area used in the guides. */
        AreaDbl guide_area = 0;

        /** Maximum xe of all items in the last bin. */
        LengthDbl xe_max = -std::numeric_limits<LengthDbl>::infinity();

        /** Maximum ye of all items in the last bin. */
        LengthDbl ye_max = -std::numeric_limits<LengthDbl>::infinity();

        /** Maximum xs of all items in the last bin. */
        LengthDbl xs_max = -std::numeric_limits<LengthDbl>::infinity();

        /** Profit. */
        Profit profit = 0;

        /** Insertions. */
        mutable std::vector<Insertion> children_insertions;
    };

    struct Parameters
    {
        /** Guide. */
        GuideId guide_id = 0;

        /** Direction. */
        Direction direction = Direction::LeftToRightThenBottomToTop;

        double maximum_approximation_ratio = 0.05;
    };

    /** Constructor. */
    BranchingScheme(
            const Instance& instance,
            const Parameters& parameters);

    /** Get instance. */
    inline const Instance& instance() const { return instance_; }

    /** Get parameters. */
    inline const Parameters& parameters() const { return parameters_; }

    /*
     * Branching scheme methods
     */

    void insertions(
            const std::shared_ptr<Node>& parent) const;

    Node child_tmp(
            const std::shared_ptr<Node>& parent,
            const Insertion& insertion) const;

    std::shared_ptr<Node> child(
            const std::shared_ptr<Node>& parent,
            const Insertion& insertion) const
    {
        return std::shared_ptr<Node>(new Node(child_tmp(parent, insertion)));
    }

    const std::shared_ptr<Node> root() const;

    std::vector<std::shared_ptr<Node>> children(
            const std::shared_ptr<Node>& parent) const;

    inline bool operator()(
            const std::shared_ptr<Node>& node_1,
            const std::shared_ptr<Node>& node_2) const;

    inline bool leaf(
            const std::shared_ptr<Node>& node) const
    {
        return node->number_of_items == instance().number_of_items();
    }

    bool bound(
            const std::shared_ptr<Node>& node_1,
            const std::shared_ptr<Node>& node_2) const;

    bool better(
            const std::shared_ptr<Node>& node_1,
            const std::shared_ptr<Node>& node_2) const;

    bool equals(
            const std::shared_ptr<Node>& node_1,
            const std::shared_ptr<Node>& node_2) const
    {
        (void)node_1;
        (void)node_2;
        return false;
    }

    /*
     * Dominances
     */

    inline bool comparable(const std::shared_ptr<Node>&) const { return true; }

    struct NodeHasher
    {
        std::hash<ItemPos> hasher;
        const BranchingScheme& branching_scheme;

        NodeHasher(const BranchingScheme& branching_scheme): branching_scheme(branching_scheme) { }

        inline bool operator()(
                const std::shared_ptr<Node>& node_1,
                const std::shared_ptr<Node>& node_2) const
        {
            //if (branching_scheme.unbounded_knapsack_)
            //    return true;
            return node_1->item_number_of_copies == node_2->item_number_of_copies;
        }

        inline std::size_t operator()(
                const std::shared_ptr<Node>& node) const
        {
            //if (branching_scheme.unbounded_knapsack_)
            //    return 0;
            size_t hash = 0;
            for (ItemPos s: node->item_number_of_copies)
                optimizationtools::hash_combine(hash, hasher(s));
            return hash;
        }
    };

    inline NodeHasher node_hasher() const { return NodeHasher(*this); }

    inline bool dominates(
            const std::shared_ptr<Node>& node_1,
            const std::shared_ptr<Node>& node_2) const
    {
        if (node_1->number_of_bins < node_2->number_of_bins)
            return true;
        if (node_1->number_of_bins > node_2->number_of_bins)
            return false;

        // Check uncovered rectangles.
        ItemPos pos_1 = node_1->uncovered_trapezoids.size() - 1;
        ItemPos pos_2 = node_2->uncovered_trapezoids.size() - 1;
        for (;;) {
            const GeneralizedTrapezoid& trapezoid_1 = node_1->uncovered_trapezoids[pos_1].trapezoid;
            const GeneralizedTrapezoid& trapezoid_2 = node_2->uncovered_trapezoids[pos_2].trapezoid;
            LengthDbl yb = std::max(trapezoid_1.y_bottom(), trapezoid_2.y_bottom());
            LengthDbl yt = std::min(trapezoid_1.y_top(), trapezoid_2.y_top());
            LengthDbl x1br = trapezoid_1.x_right(yb);
            LengthDbl x1tr = trapezoid_1.x_right(yt);
            LengthDbl x2br = trapezoid_2.x_right(yb);
            LengthDbl x2tr = trapezoid_2.x_right(yt);
            if (strictly_greater(x1br, x2br))
                return false;
            if (strictly_greater(x1tr, x2tr))
                return false;
            if (pos_1 == 0 && pos_2 == 0)
                break;
            if (equal(trapezoid_1.y_bottom(), trapezoid_2.y_bottom())) {
                if (pos_1 == 0 || pos_2 == 0)
                    break;
                pos_1--;
                pos_2--;
            } else if (strictly_lesser(trapezoid_1.y_bottom(), trapezoid_2.y_bottom())) {
                pos_2--;
            } else {
                pos_1--;
            }
        }

        // Check extra rectangles.
        if (node_1->extra_trapezoids.size() != node_2->extra_trapezoids.size())
            return false;
        for (ItemPos extra_trapezoid_pos = 0;
                extra_trapezoid_pos < (ItemPos)node_1->extra_trapezoids.size();
                ++extra_trapezoid_pos) {
            const GeneralizedTrapezoid& trapezoid_1 = node_1->extra_trapezoids[extra_trapezoid_pos].trapezoid;
            const GeneralizedTrapezoid& trapezoid_2 = node_2->extra_trapezoids[extra_trapezoid_pos].trapezoid;
            if (!equal(trapezoid_1.y_bottom(), trapezoid_2.y_bottom()))
                return false;
            if (!equal(trapezoid_1.y_top(), trapezoid_2.y_top()))
                return false;
            if (!equal(trapezoid_1.x_bottom_left(), trapezoid_2.x_bottom_left()))
                return false;
            if (!equal(trapezoid_1.x_bottom_right(), trapezoid_2.x_bottom_right()))
                return false;
            if (!equal(trapezoid_1.x_top_left(), trapezoid_2.x_top_left()))
                return false;
            if (!equal(trapezoid_1.x_top_right(), trapezoid_2.x_top_right()))
                return false;
        }

        return true;
    }

    /*
     * Outputs
     */

    std::string display(const std::shared_ptr<Node>& node) const
    {
        std::stringstream ss;
        ss << node->profit;
        return ss.str();
    }

    Solution to_solution(
            const std::shared_ptr<Node>& node) const;

    std::string to_svg(
            const std::shared_ptr<Node>& node) const;

    void write_svg(
            const std::shared_ptr<Node>& node,
            const std::string& file_path) const;

    nlohmann::json json_export_init() const;

    nlohmann::json json_export(
            const std::shared_ptr<Node>& node) const;

private:

    /** Instance. */
    const Instance& instance_;

    /** Parameters. */
    Parameters parameters_;

    SimplifiedInstance simplified_instance_;

    std::vector<AreaDbl> item_types_convex_hull_area_;

    DirectionData directions_data_[8];

    mutable std::vector<std::vector<std::unordered_map<Angle, Counter>>> json_items_init_ids_;

    mutable std::vector<Counter> json_bins_init_ids_;

    mutable Counter node_id_ = 0;

    mutable std::vector<GeneralizedTrapezoid> uncovered_trapezoids_cur_;

    /*
     * Private methods
     */

    Shape convert_shape(
            Shape shape,
            Direction direction) const;

    Shape convert_shape(
            const Shape& shape,
            Angle angle,
            bool mirror) const;

    Shape convert_shape(
            const Shape& shape,
            Angle angle,
            bool mirror,
            Direction direction) const;

    Point convert_point_back(
            const Point& point,
            Direction direction) const;

    /** Get the percentage of item inserted into a node. */
    inline double item_percentage(const Node& node) const { return (double)node.number_of_items / instance().number_of_items(); }

    /** Get the mean item area of a node; */
    inline double mean_item_area(const Node& node) const { return (double)node.item_area / node.number_of_items; }

    /** Get the mean remaining item area of a node. */
    inline double mean_remaining_item_area(const Node& node) const { return (double)remaining_item_area(node) / (instance().number_of_items() - node.number_of_items); }

    /** Get the remaining item area of a node. */
    inline double remaining_item_area(const Node& node) const { return instance().item_area() - node.item_area; }

    /** Get the area load of a node. */
    inline double area_load(const Node& node) const { return (double)node.item_area / instance().bin_area(); }

    std::vector<UncoveredTrapezoid> add_trapezoid_to_skyline(
            const std::vector<UncoveredTrapezoid>& uncovered_trapezoids,
            ItemTypeId item_type_id,
            ItemShapePos item_shape_pos,
            ItemShapeTrapezoidPos item_shape_trapezoid_pos,
            const GeneralizedTrapezoid& trapezoid) const;

    void check_skyline(
            const std::vector<UncoveredTrapezoid>& uncovered_trapezoids) const;

    void update_extra_trapezoids(Node& node) const;

    LengthDbl compute_direction(
            const ShapeElement& supporting_element,
            const ShapeElement& supported_element) const;

    bool update_position(
            Insertion& insertion,
            const Shape& supporting_shape,
            const Shape& supported_shape,
            const GeneralizedTrapezoid& trapezoid_to_avoid,
            GeneralizedTrapezoid& current_trapezoid) const;

    /** Update insertion. */
    void update_insertion(
            const std::shared_ptr<Node>& parent,
            const Insertion& insertion) const;

    /** Insertion of one item. */
    void insertion_trapezoid_set(
            const std::shared_ptr<Node>& parent,
            TrapezoidSetId trapezoid_set_id,
            ShapePos supporting_part_pos,
            LengthDbl supporting_part_x,
            LengthDbl supporting_part_y,
            ShapePos supported_part_pos,
            Direction new_bin_direction) const;

};

std::ostream& operator<<(
        std::ostream& os,
        const BranchingScheme::UncoveredTrapezoid& uncovered_trapezoids);

std::ostream& operator<<(
        std::ostream& os,
        const BranchingScheme::Insertion& insertion);

std::ostream& operator<<(
        std::ostream& os,
        const BranchingScheme::Node& node);

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Inlined methods ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline bool BranchingScheme::operator()(
        const std::shared_ptr<Node>& node_1,
        const std::shared_ptr<Node>& node_2) const
{
    switch(parameters_.guide_id) {
    case 0: {
        if (node_1->guide_area == 0)
            return node_2->guide_area != 0;
        if (node_2->guide_area == 0)
            return false;
        double guide_1 = (double)node_1->guide_area / node_1->item_convex_hull_area;
        double guide_2 = (double)node_2->guide_area / node_2->item_convex_hull_area;
        if (guide_1 != guide_2)
            return guide_1 < guide_2;
        break;
    } case 1: {
        if (node_1->guide_area == 0)
            return node_2->guide_area != 0;
        if (node_2->guide_area == 0)
            return false;
        if (node_1->number_of_items == 0)
            return node_2->number_of_items != 0;
        if (node_2->number_of_items == 0)
            return true;
        double guide_1 = (double)node_1->guide_area
            / node_1->item_convex_hull_area
            / node_1->item_convex_hull_area;
        double guide_2 = (double)node_2->guide_area
            / node_2->item_convex_hull_area
            / node_2->item_convex_hull_area;
        if (guide_1 != guide_2)
            return guide_1 < guide_2;
        break;
    } case 2: {
        if (node_1->number_of_items == 0)
            return node_2->number_of_items != 0;
        if (node_2->number_of_items == 0)
            return true;
        double guide_1 = (double)(node_1->xe_max * node_1->ye_max) / node_1->item_convex_hull_area;
        double guide_2 = (double)(node_2->xe_max * node_2->ye_max) / node_2->item_convex_hull_area;
        if (guide_1 != guide_2)
            return guide_1 < guide_2;
        break;
    } case 3: {
        if (node_1->number_of_items == 0)
            return node_2->number_of_items != 0;
        if (node_2->number_of_items == 0)
            return true;
        double guide_1 = (double)(node_1->xe_max * node_1->ye_max) / node_1->item_convex_hull_area
            / node_1->item_convex_hull_area;
        double guide_2 = (double)(node_2->xe_max * node_2->ye_max) / node_2->item_convex_hull_area
            / node_2->item_convex_hull_area;
        if (guide_1 != guide_2)
            return guide_1 < guide_2;
        break;
    } case 4: {
        if (node_1->profit == 0)
            return node_2->profit != 0;
        if (node_2->profit == 0)
            return true;
        double guide_1 = (double)node_1->guide_area / node_1->profit;
        double guide_2 = (double)node_2->guide_area / node_2->profit;
        if (guide_1 != guide_2)
            return guide_1 < guide_2;
        break;
    } case 5: {
        if (node_1->profit == 0)
            return node_2->profit != 0;
        if (node_2->profit == 0)
            return true;
        if (node_1->number_of_items == 0)
            return node_2->number_of_items != 0;
        if (node_2->number_of_items == 0)
            return true;
        double guide_1 = (double)node_1->guide_area
            / node_1->profit
            / mean_item_area(*node_1);
        double guide_2 = (double)node_2->guide_area
            / node_2->profit
            / mean_item_area(*node_2);
        if (guide_1 != guide_2)
            return guide_1 < guide_2;
        break;
    }
    }
    return node_1->id < node_2->id;
}

}
}
