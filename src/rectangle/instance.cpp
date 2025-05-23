#include "packingsolver/rectangle/instance.hpp"

#include <iostream>

using namespace packingsolver;
using namespace packingsolver::rectangle;

std::istream& rectangle::operator>>(
        std::istream& in,
        Direction& o)
{
    std::string token;
    in >> token;
    if (token == "x"
            || token == "X") {
        o = Direction::X;
    } else if (token == "y"
            || token == "Y") {
        o = Direction::Y;
    } else if (token == "any"
            || token == "Any") {
        o = Direction::Any;
    } else  {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

std::ostream& rectangle::operator<<(
        std::ostream& os,
        Direction o)
{
    switch (o) {
    case Direction::X: {
        os << "X";
        break;
    } case Direction::Y: {
        os << "Y";
        break;
    } case Direction::Any: {
        os << "Any";
        break;
    }
    }
    return os;
}

std::istream& rectangle::operator>>(
        std::istream& in,
        UnloadingConstraint& unloading_constraint)
{
    std::string token;
    in >> token;
    if (token == "none"
            || token == "NONE"
            || token == "None") {
        unloading_constraint = UnloadingConstraint::None;
    } else if (token == "only-x-movements"
            || token == "ONLY_X_MOVEMENTS"
            || token == "OnlyXMovements") {
        unloading_constraint = UnloadingConstraint::OnlyXMovements;
    } else if (token == "only-y-movements"
            || token == "ONLY_Y_MOVEMENTS"
            || token == "OnlyYMovements") {
        unloading_constraint = UnloadingConstraint::OnlyYMovements;
    } else if (token == "increasing-x"
            || token == "INCREASING_X"
            || token == "IncreasingX") {
        unloading_constraint = UnloadingConstraint::IncreasingX;
    } else if (token == "increasing-y"
            || token == "INCREASING_Y"
            || token == "IncreasingY") {
        unloading_constraint = UnloadingConstraint::IncreasingY;
    } else  {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

std::ostream& rectangle::operator<<(
        std::ostream& os,
        UnloadingConstraint unloading_constraint)
{
    switch (unloading_constraint) {
    case UnloadingConstraint::None: {
        os << "None";
        break;
    } case UnloadingConstraint::OnlyXMovements: {
        os << "OnlyXMovements";
        break;
    } case UnloadingConstraint::OnlyYMovements: {
        os << "OnlyYMovements";
        break;
    } case UnloadingConstraint::IncreasingX: {
        os << "IncreasingX";
        break;
    } case UnloadingConstraint::IncreasingY: {
        os << "IncreasingY";
        break;
    }
    }
    return os;
}

bool rectangle::rect_intersection(
        Point p1, Rectangle r1, Point p2, Rectangle r2)
{
    return p1.x + r1.x > p2.x
        && p2.x + r2.x > p1.x
        && p1.y + r1.y > p2.y
        && p2.y + r2.y > p1.y;
}

bool rectangle::rect_intersection(
        Length x1, Length x2, Length y1, Length y2,
        Length x3, Length x4, Length y3, Length y4)
{
    return x2 > x3
        && x4 > x1
        && y2 > y3
        && y4 > y1;
}

std::ostream& packingsolver::rectangle::operator<<(
        std::ostream& os,
        Point xy)
{
    os << xy.x << " " << xy.y;
    return os;
}

std::ostream& packingsolver::rectangle::operator<<(
        std::ostream& os,
        Rectangle r)
{
    os << r.x << " " << r.y;
    return os;
}

std::ostream& packingsolver::rectangle::operator<<(
        std::ostream& os,
        const ItemType& item_type)
{
    os
        << "x " << item_type.rect.x
        << " y " << item_type.rect.y
        << " profit " << item_type.profit
        << " copies " << item_type.copies
        << " group_id " << item_type.group_id
        << " oriented " << item_type.oriented
        ;
    return os;
}

std::ostream& packingsolver::rectangle::operator<<(
        std::ostream& os,
        const BinType& bin_type)
{
    os
        << "x " << bin_type.rect.x
        << " y " << bin_type.rect.y
        << " copies " << bin_type.copies
        ;
    return os;
}

std::ostream& packingsolver::rectangle::operator<<(
        std::ostream& os,
        const Defect& defect)
{
    os
        << "bin_type_id " << defect.bin_type_id
        << " pos " << defect.pos
        << " rect " << defect.rect
        ;
    return os;
}

std::ostream& Instance::format(
        std::ostream& os,
        int verbosity_level) const
{
    if (verbosity_level >= 1) {
        os
            << "Objective:             " << objective() << std::endl
            << "Number of item types:  " << number_of_item_types() << std::endl
            << "Number of items:       " << number_of_items() << std::endl
            << "Number of bin types:   " << number_of_bin_types() << std::endl
            << "Number of bins:        " << number_of_bins() << std::endl
            << "Number of groups:      " << number_of_groups() << std::endl
            << "Number of defects:     " << number_of_defects() << std::endl
            << "Unloading constraint:  " << unloading_constraint() << std::endl
            << "Total item area:       " << item_area() << std::endl
            << "Total item width:      " << total_item_width() << std::endl
            << "Total item height:     " << total_item_height() << std::endl
            << "Smallest item width:   " << smallest_item_width() << std::endl
            << "Smallest item height:  " << smallest_item_height() << std::endl
            << "Total item profit:     " << item_profit() << std::endl
            << "Largest item profit:   " << largest_item_profit() << std::endl
            << "Total item weight:     " << item_weight() << std::endl
            << "Largest item copies:   " << largest_item_copies() << std::endl
            << "Total bin area:        " << bin_area() << std::endl
            << "Total bin weight:      " << bin_weight() << std::endl
            << "Largest bin cost:      " << largest_bin_cost() << std::endl
            ;
    }

    if (verbosity_level >= 2) {
        os
            << std::endl
            << std::setw(12) << "Bin type"
            << std::setw(12) << "Width"
            << std::setw(12) << "Height"
            << std::setw(12) << "Cost"
            << std::setw(12) << "Copies"
            << std::setw(12) << "Copies min"
            << std::endl
            << std::setw(12) << "--------"
            << std::setw(12) << "-----"
            << std::setw(12) << "------"
            << std::setw(12) << "----"
            << std::setw(12) << "------"
            << std::setw(12) << "----------"
            << std::endl;
        for (BinTypeId bin_type_id = 0;
                bin_type_id < number_of_bin_types();
                ++bin_type_id) {
            const BinType& bin_type = this->bin_type(bin_type_id);
            os
                << std::setw(12) << bin_type_id
                << std::setw(12) << bin_type.rect.x
                << std::setw(12) << bin_type.rect.y
                << std::setw(12) << bin_type.cost
                << std::setw(12) << bin_type.copies
                << std::setw(12) << bin_type.copies_min
                << std::endl;
        }

        if (number_of_defects() > 0) {
            os
                << std::endl
                << std::setw(12) << "Defect"
                << std::setw(12) << "Bin type"
                << std::setw(12) << "X"
                << std::setw(12) << "Y"
                << std::setw(12) << "width"
                << std::setw(12) << "Height"
                << std::endl
                << std::setw(12) << "------"
                << std::setw(12) << "--------"
                << std::setw(12) << "-"
                << std::setw(12) << "-"
                << std::setw(12) << "-----"
                << std::setw(12) << "------"
                << std::endl;
            for (BinTypeId bin_type_id = 0;
                    bin_type_id < number_of_bin_types();
                    ++bin_type_id) {
                const BinType& bin_type = this->bin_type(bin_type_id);
                for (DefectId defect_id = 0;
                        defect_id < (DefectId)bin_type.defects.size();
                        ++defect_id) {
                    const Defect& defect = bin_type.defects[defect_id];
                    os
                        << std::setw(12) << defect_id
                        << std::setw(12) << defect.bin_type_id
                        << std::setw(12) << defect.pos.x
                        << std::setw(12) << defect.pos.y
                        << std::setw(12) << defect.rect.x
                        << std::setw(12) << defect.rect.y
                        << std::endl;
                }
            }
        }

        os
            << std::endl
            << std::setw(12) << "Item type"
            << std::setw(12) << "Width"
            << std::setw(12) << "Height"
            << std::setw(12) << "Profit"
            << std::setw(12) << "Copies"
            << std::setw(12) << "Oriented"
            << std::setw(12) << "Group id"
            << std::setw(12) << "Weight"
            << std::endl
            << std::setw(12) << "---------"
            << std::setw(12) << "-----"
            << std::setw(12) << "------"
            << std::setw(12) << "------"
            << std::setw(12) << "------"
            << std::setw(12) << "--------"
            << std::setw(12) << "--------"
            << std::setw(12) << "------"
            << std::endl;
        for (ItemTypeId item_type_id = 0;
                item_type_id < number_of_item_types();
                ++item_type_id) {
            const ItemType& item_type = this->item_type(item_type_id);
            os
                << std::setw(12) << item_type_id
                << std::setw(12) << item_type.rect.x
                << std::setw(12) << item_type.rect.y
                << std::setw(12) << item_type.profit
                << std::setw(12) << item_type.copies
                << std::setw(12) << item_type.oriented
                << std::setw(12) << item_type.group_id
                << std::setw(12) << item_type.weight
                << std::endl;
        }
    }

    return os;
}

void Instance::write(
        const std::string& instance_path) const
{
    // Export items.
    std::string items_path = instance_path + "_items.csv";
    std::ofstream f_items(items_path);
    if (!f_items.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + items_path + "\".");
    }
    f_items <<
        "ID,"
        "WIDTH,"
        "HEIGHT,"
        "PROFIT,"
        "COPIES,"
        "ORIENTED,"
        "GROUP_ID" << std::endl;
    for (ItemTypeId item_type_id = 0;
            item_type_id < number_of_item_types();
            ++item_type_id) {
        const ItemType& item_type = this->item_type(item_type_id);
        f_items
            << item_type_id << ","
            << item_type.rect.x << ","
            << item_type.rect.y << ","
            << item_type.profit << ","
            << item_type.copies << ","
            << item_type.oriented << ","
            << item_type.group_id << std::endl;
    }

    // Export bins.
    std::string bins_path = instance_path + "_bins.csv";
    std::ofstream f_bins(bins_path);
    if (!f_bins.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + bins_path + "\".");
    }
    f_bins <<
        "ID,"
        "WIDTH,"
        "HEIGHT,"
        "COST,"
        "COPIES,"
        "COPIES_MIN" << std::endl;
    for (BinTypeId bin_type_id = 0;
            bin_type_id < number_of_bin_types();
            ++bin_type_id) {
        const BinType& bin_type = this->bin_type(bin_type_id);
        f_bins
            << bin_type_id << ","
            << bin_type.rect.x << ","
            << bin_type.rect.y << ","
            << bin_type.cost << ","
            << bin_type.copies << ","
            << bin_type.copies_min << std::endl;
    }

    // Export defects.
    if (number_of_defects() > 0) {
        std::string defects_path = instance_path + "_defects.csv";
        std::ofstream f_defects(defects_path);
        if (number_of_defects() > 0 && !f_defects.good()) {
            throw std::runtime_error(
                    "Unable to open file \"" + defects_path + "\".");
        }
        f_defects <<
            "ID,"
            "BIN,"
            "X,"
            "Y,"
            "WIDTH,"
            "HEIGHT" << std::endl;
        for (BinTypeId bin_type_id = 0;
                bin_type_id < number_of_bin_types();
                ++bin_type_id) {
            const BinType& bin_type = this->bin_type(bin_type_id);
            for (DefectId defect_id = 0;
                    defect_id < (DefectId)bin_type.defects.size();
                    ++defect_id) {
                const Defect& defect = bin_type.defects[defect_id];
                f_defects
                    << defect_id << ","
                    << bin_type_id << ","
                    << defect.pos.x << ","
                    << defect.pos.y << ","
                    << defect.rect.x << ","
                    << defect.rect.y << std::endl;
            }
        }
    }

    // Export parameters.
    std::string parameters_path = instance_path + "_parameters.csv";
    std::ofstream f_parameters(parameters_path);
    if (!f_parameters.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + parameters_path + "\".");
    }
    f_parameters << "NAME,VALUE" << std::endl
        << "objective," << objective() << std::endl
        << "unloading_constraint," << unloading_constraint() << std::endl;

}
