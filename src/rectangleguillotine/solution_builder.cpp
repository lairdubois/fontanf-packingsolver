#include "rectangleguillotine/solution_builder.hpp"

#include "optimizationtools/utils/utils.hpp"

using namespace packingsolver;
using namespace packingsolver::rectangleguillotine;

void SolutionBuilder::add_bin(
        BinTypeId bin_type_id,
        BinPos copies,
        CutOrientation first_cut_orientation)
{
    if (bin_type_id < 0
            || bin_type_id >= solution_.instance().number_of_bin_types()) {
        throw std::invalid_argument(
                "packingsolver::rectangleguillotine::SolutionBuilder::add_bin: "
                "invalid 'bin_type_id'; "
                "bin_type_id: " + std::to_string(bin_type_id) + "; "
                "instance().number_of_bin_types(): " + std::to_string(solution_.instance().number_of_bin_types()) + ".");
    }

    //std::cout << "add_bin bin_type_id " << bin_type_id << " copies " << copies << " first_cut_orientation " << first_cut_orientation << std::endl;
    const BinType& bin_type = solution_.instance().bin_type(bin_type_id);
    SolutionBin bin;
    bin.bin_type_id = bin_type_id;
    bin.copies = copies;
    bin.first_cut_orientation = first_cut_orientation;

    SolutionNodeId root_id = 0;
    SolutionNode root;
    root.f = -1;
    root.d = 0;
    root.l = 0;
    root.r = bin_type.rect.w;
    root.b = 0;
    root.t = bin_type.rect.h;
    root.item_type_id = bin_type_id;
    bin.nodes.push_back(root);
    bool left_trim = (bin_type.left_trim_type == TrimType::Hard && bin_type.left_trim > 0);
    bool right_trim = (bin_type.right_trim_type == TrimType::Hard && bin_type.right_trim > 0);
    bool bottom_trim = (bin_type.bottom_trim_type == TrimType::Hard && bin_type.bottom_trim > 0);
    bool top_trim = (bin_type.top_trim_type == TrimType::Hard && bin_type.top_trim > 0);

    // Trims.
    if (left_trim
            || right_trim
            || bottom_trim
            || top_trim) {

        if (left_trim && bottom_trim) {
            SolutionNode node;
            node.d = -1;
            node.f = 0;
            node.l = 0;
            node.r = bin_type.left_trim;
            node.b = 0;
            node.t = bin_type.bottom_trim;
            node.item_type_id = -1;
            root.children.push_back(bin.nodes.size());
            bin.nodes.push_back(node);
        }

        if (left_trim && top_trim) {
            SolutionNode node;
            node.d = -1;
            node.f = 0;
            node.l = 0;
            node.r = bin_type.left_trim;
            node.b = bin_type.rect.h - bin_type.top_trim;
            node.t = bin_type.rect.h;
            node.item_type_id = -1;
            root.children.push_back(bin.nodes.size());
            bin.nodes.push_back(node);
        }

        if (right_trim && bottom_trim) {
            SolutionNode node;
            node.d = -1;
            node.f = 0;
            node.l = bin_type.rect.w - bin_type.right_trim;
            node.r = bin_type.rect.w;
            node.b = 0;
            node.t = bin_type.bottom_trim;
            node.item_type_id = -1;
            root.children.push_back(bin.nodes.size());
            bin.nodes.push_back(node);
        }

        if (right_trim && bin_type.top_trim) {
            SolutionNode node;
            node.d = -1;
            node.f = 0;
            node.l = bin_type.rect.w - bin_type.right_trim;
            node.r = bin_type.rect.w;
            node.b = bin_type.rect.h - bin_type.top_trim;
            node.t = bin_type.rect.h;
            node.item_type_id = -1;
            root.children.push_back(bin.nodes.size());
            bin.nodes.push_back(node);
        }

        if (left_trim) {
            SolutionNode node;
            node.d = -1;
            node.f = 0;
            node.l = 0;
            node.r = bin_type.left_trim;
            node.b = (!bottom_trim)? 0: bin_type.bottom_trim;
            node.t = (!top_trim)? bin_type.rect.h: bin_type.rect.h - bin_type.top_trim;
            node.item_type_id = -1;
            root.children.push_back(bin.nodes.size());
            bin.nodes.push_back(node);
        }

        if (bottom_trim) {
            SolutionNode node;
            node.d = -1;
            node.f = 0;
            node.l = (!left_trim)? 0: bin_type.left_trim;
            node.r = (!right_trim)? bin_type.rect.w: bin_type.rect.w - bin_type.right_trim;
            node.b = 0;
            node.t = bin_type.bottom_trim;
            node.item_type_id = -1;
            root.children.push_back(bin.nodes.size());
            bin.nodes.push_back(node);
        }

        if (right_trim) {
            SolutionNode node;
            node.d = -1;
            node.f = 0;
            node.l = bin_type.rect.w - bin_type.right_trim;
            node.r = bin_type.rect.w;
            node.b = (!bottom_trim)? 0: bin_type.bottom_trim;
            node.t = (!top_trim)? bin_type.rect.h: bin_type.rect.h - bin_type.top_trim;
            node.item_type_id = -1;
            root.children.push_back(bin.nodes.size());
            bin.nodes.push_back(node);
        }

        if (top_trim) {
            SolutionNode node;
            node.d = -1;
            node.f = 0;
            node.l = (!left_trim)? 0: bin_type.left_trim;
            node.r = (!right_trim)? bin_type.rect.w: bin_type.rect.w - bin_type.right_trim;
            node.b = bin_type.rect.h - bin_type.top_trim;
            node.t = bin_type.rect.h;
            node.item_type_id = -1;
            root.children.push_back(bin.nodes.size());
            bin.nodes.push_back(node);
        }

        SolutionNode node;
        node.d = 0;
        node.f = 0;
        node.l = (!left_trim)? 0: bin_type.left_trim;
        node.r = (!right_trim)? bin_type.rect.w: bin_type.rect.w - bin_type.right_trim;
        node.b = (!bottom_trim)? 0: bin_type.bottom_trim;
        node.t = (!top_trim)? bin_type.rect.h: bin_type.rect.h - bin_type.top_trim;
        node.item_type_id = -1;
        root.children.push_back(bin.nodes.size());
        bin.nodes.push_back(node);
    }

    solution_.bins_.push_back(bin);
}

void SolutionBuilder::set_last_node_item(
        ItemTypeId item_type_id)
{
    if (solution_.bins_.empty()) {
        throw std::logic_error(
                "packingsolver::rectangleguillotine::SolutionBuilder::set_last_node_item: "
                "at least one bin must have been added to the solution.");
    }
    if (item_type_id < 0
            || item_type_id >= solution_.instance().number_of_item_types()) {
        throw std::logic_error(
                "packingsolver::rectangleguillotine::SolutionBuilder::set_last_node_item: "
                "wrong 'item_type_id' value; "
                "item_type_id: " + std::to_string(item_type_id) + "; "
                "solution_.instance().number_of_item_types(): " + std::to_string(solution_.instance().number_of_item_types()) + ".");
    }

    const ItemType& item_type = solution_.instance().item_type(item_type_id);
    SolutionBin& bin = solution_.bins_.back();
    SolutionNode& node = bin.nodes.back();

    // Check node dimensions.
    bool ok = (
            (node.r - node.l == item_type.rect.w && node.t - node.b == item_type.rect.h)
            || (node.r - node.l == item_type.rect.h && node.t - node.b == item_type.rect.w));
    if (!ok) {
        throw std::logic_error(
                "packingsolver::rectangleguillotine::SolutionBuilder::set_last_node_item: "
                "wrong item dimensions; "
                "item_type_id: " + std::to_string(item_type_id) + "; "
                "item_type.rect.w: " + std::to_string(item_type.rect.w) + "; "
                "item_type.rect.h: " + std::to_string(item_type.rect.h) + "; "
                "node.l: " + std::to_string(node.l) + "; "
                "node.r: " + std::to_string(node.r) + "; "
                "node.b: " + std::to_string(node.b) + "; "
                "node.t: " + std::to_string(node.t) + ".");
    }

    node.item_type_id = item_type_id;
}

void SolutionBuilder::add_node(
        Depth depth,
        Length cut_position)
{
    Length cut_thickness = solution_.instance().parameters().cut_thickness;
    //std::cout << "add_node depth " << depth << " cut_position " << cut_position << std::endl;
    if (solution_.bins_.empty()) {
        throw std::logic_error(
                "packingsolver::rectangleguillotine::SolutionBuilder::add_node: "
                "at least one bin must have been added to the solution.");
    }
    if (depth < 1) {
        throw std::logic_error(
                "packingsolver::rectangleguillotine::SolutionBuilder::add_node: "
                "wrong 'depth'; "
                "depth: " + std::to_string(depth) + ".");
    }
    SolutionBin& bin = solution_.bins_.back();
    //std::cout << "bin.nodes.size() " << bin.nodes.size() << std::endl;

    SolutionNodeId parent_id = bin.nodes.size() - 1;
    //std::cout << "parent_id " << parent_id << " depth " << bin.nodes[parent_id].d << std::endl;
    if (depth > bin.nodes[parent_id].d + 1) {
        throw std::logic_error(
                "packingsolver::rectangleguillotine::SolutionBuilder::add_node: "
                "wrong depth; "
                "depth: " + std::to_string(depth) + "; "
                "parent.depth: " + std::to_string(bin.nodes[parent_id].d) + ".");
    }

    while (bin.nodes[parent_id].d != depth - 1) {
        SolutionNode& parent = bin.nodes[parent_id];
        // Add a final child to the last node if necessary.
        if (!parent.children.empty()) {
            const SolutionNode& brother = bin.nodes[parent.children.back()];
            if (parent.r > brother.r + cut_thickness) {
                SolutionNode child;
                child.f = parent_id;
                child.d = parent.d + 1;
                child.l = brother.r + cut_thickness;
                child.r = parent.r;
                child.b = parent.b;
                child.t = parent.t;
                child.item_type_id = -1;
                parent.children.push_back(bin.nodes.size());
                bin.nodes.push_back(child);
                //std::cout << "new node " << child << std::endl;
            } else if (parent.t > brother.t + cut_thickness) {
                SolutionNode child;
                child.f = parent_id;
                child.d = parent.d + 1;
                child.l = parent.l;
                child.r = parent.r;
                child.b = brother.t + cut_thickness;
                child.t = parent.t;
                child.item_type_id = -1;
                parent.children.push_back(bin.nodes.size());
                bin.nodes.push_back(child);
                //std::cout << "new node " << child << std::endl;
            }
        }
        parent_id = bin.nodes[parent_id].f;
        //std::cout << "parent_id " << parent_id << " depth " << bin.nodes[parent_id].d << std::endl;
    }

    SolutionNode& parent = bin.nodes[parent_id];
    if (parent.f != -1 && parent.item_type_id >= 0) {
        throw std::logic_error(
                "packingsolver::rectangleguillotine::SolutionBuilder::add_node: "
                "cannot add a child to a node with an item.");
    }
    if (parent.f != -1)
        parent.item_type_id = -2;

    SolutionNode child;
    child.f = parent_id;
    child.d = parent.d + 1;
    if ((bin.first_cut_orientation == CutOrientation::Vertical
                && depth % 2 == 1)
            || (bin.first_cut_orientation == CutOrientation::Horizontal
                && depth % 2 == 0)) {
        child.l = (parent.children.empty())? parent.l: bin.nodes[parent.children.back()].r + cut_thickness;
        child.r = cut_position;
        child.b = parent.b;
        child.t = parent.t;
        if (child.r <= child.l) {
            throw std::logic_error(
                    "packingsolver::rectangleguillotine::SolutionBuilder::add_node: "
                    "'cut_position' is too small; "
                    "depth: " + std::to_string(depth) + "; "
                    "cut_position: " + std::to_string(cut_position) + "; "
                    "child.l: " + std::to_string(child.l) + "; "
                    "child.r: " + std::to_string(child.r) + ".");
        }
        if (child.r > parent.r) {
            throw std::logic_error(
                    "packingsolver::rectangleguillotine::SolutionBuilder::add_node: "
                    "'cut_position' is too large; "
                    "depth: " + std::to_string(depth) + "; "
                    "cut_position: " + std::to_string(cut_position) + "; "
                    "parent.r: " + std::to_string(parent.r) + "; "
                    "child.r: " + std::to_string(child.r) + ".");
        }
    } else {
        child.l = parent.l;
        child.r = parent.r;
        child.b = (parent.children.empty())? parent.b: bin.nodes[parent.children.back()].t + cut_thickness;
        child.t = cut_position;
        if (child.t <= child.b) {
            throw std::logic_error(
                    "packingsolver::rectangleguillotine::SolutionBuilder::add_node: "
                    "'cut_position' is too small; "
                    "depth: " + std::to_string(depth) + "; "
                    "cut_position: " + std::to_string(cut_position) + "; "
                    "child.b: " + std::to_string(child.b) + "; "
                    "child.t: " + std::to_string(child.t) + ".");
        }
        if (child.t > parent.t) {
            throw std::logic_error(
                    "packingsolver::rectangleguillotine::SolutionBuilder::add_node: "
                    "'cut_position' is too large; "
                    "depth: " + std::to_string(depth) + "; "
                    "cut_position: " + std::to_string(cut_position) + "; "
                    "parent.t: " + std::to_string(parent.t) + "; "
                    "child.t: " + std::to_string(child.t) + ".");
        }
    }
    child.item_type_id = -1;
    parent.children.push_back(bin.nodes.size());
    bin.nodes.push_back(child);
}

void SolutionBuilder::read(
        const std::string& certificate_path)
{
    std::ifstream f(certificate_path);
    if (!f.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + certificate_path + "\".");
    }

    std::string tmp;
    std::vector<std::string> line;
    std::vector<std::string> labels;

    getline(f, tmp);
    labels = optimizationtools::split(tmp, ',');
    std::vector<std::vector<SolutionNode>> nodes;
    SolutionNodeId offset = 0;
    while (getline(f, tmp)) {
        line = optimizationtools::split(tmp, ',');

        SolutionNode node;
        Length width = -1;
        Length height = -1;
        BinPos bin_pos = -1;
        for (Counter i = 0; i < (Counter)line.size(); ++i) {
            if (labels[i] == "NODE_ID") {
            } else if (labels[i] == "PLATE_ID") {
                bin_pos = (BinPos)std::stol(line[i]);
            } else if (labels[i] == "X") {
                node.l = (Length)std::stol(line[i]);
            } else if (labels[i] == "Y") {
                node.b = (Length)std::stol(line[i]);
            } else if (labels[i] == "WIDTH") {
                width = (Length)std::stol(line[i]);
            } else if (labels[i] == "HEIGHT") {
                height = (Length)std::stol(line[i]);
            } else if (labels[i] == "TYPE") {
                node.item_type_id = (ItemTypeId)std::stol(line[i]);
            } else if (labels[i] == "CUT") {
                node.d = (Depth)std::stol(line[i]);
            } else if (labels[i] == "PARENT") {
                if (line[i] == "") {
                    node.f = -1;
                } else {
                    node.f = (SolutionNodeId)std::stol(line[i]) - offset;
                }
            }
            node.r = node.l + width;
            node.t = node.b + height;
        }

        // Skip defects.
        if (node.item_type_id == -4)
            continue;

        if (nodes.size() <= bin_pos) {
            if (nodes.size() > 0)
                offset += nodes.back().size();
            nodes.push_back({});
        }
        nodes[bin_pos].push_back(node);
    }

    for (BinPos bin_pos = 0;
            bin_pos < (BinPos)nodes.size();
            ++bin_pos) {

        BinTypeId bin_type_id = -1;
        CutOrientation first_cut_orientation = CutOrientation::Any;
        for (SolutionNodeId node_id = 0;
                node_id < (SolutionNodeId)nodes[bin_pos].size();
                ++node_id) {
            const SolutionNode& node = nodes[bin_pos][node_id];
            if (node.f == -1 && bin_type_id < 0)
                bin_type_id = node.item_type_id;
            if (node.d == 1) {
                const SolutionNode& parent = nodes[bin_pos][node.f];
                if (node.b == parent.b && node.t == parent.t) {
                    first_cut_orientation = CutOrientation::Vertical;
                } else if (node.l == parent.l && node.r == parent.r) {
                    first_cut_orientation = CutOrientation::Horizontal;
                } else {
                    throw std::logic_error("");
                }
                break;
            }
        }
        //std::cout << "bin_type_id " << bin_type_id << std::endl;
        //std::cout << "first_cut_orientation " << first_cut_orientation << std::endl;
        if (bin_type_id < 0)
            bin_type_id = solution_.instance().bin_type_id(bin_pos);
        add_bin(bin_type_id, 1, first_cut_orientation);

        for (SolutionNodeId node_id = 0;
                node_id < (SolutionNodeId)nodes[bin_pos].size();
                ++node_id) {
            const SolutionNode& node = nodes[bin_pos][node_id];
            //std::cout << node << std::endl;
            if (node.d <= 0)
                continue;
            if ((first_cut_orientation == CutOrientation::Vertical
                        && node.d % 2 == 1)
                    || (first_cut_orientation == CutOrientation::Horizontal
                        && node.d % 2 == 0)) {
                add_node(node.d, node.r);
            } else {
                add_node(node.d, node.t);
            }
            if (node.item_type_id >= 0)
                set_last_node_item(node.item_type_id);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Build /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Solution SolutionBuilder::build()
{
    // Finish bins.
    Length cut_thickness = solution_.instance().parameters().cut_thickness;
    for (BinPos bin_pos = 0;
            bin_pos < solution_.number_of_different_bins();
            ++bin_pos) {
        SolutionBin& bin = solution_.bins_[bin_pos];
        SolutionNodeId parent_id = bin.nodes.size() - 1;
        while (parent_id != -1) {
            SolutionNode& parent = bin.nodes[parent_id];
            // Add a final child to the last node if necessary.
            if (!parent.children.empty()) {
                const SolutionNode& brother = bin.nodes[parent.children.back()];
                if (parent.r > brother.r + cut_thickness) {
                    SolutionNode child;
                    child.f = parent_id;
                    child.d = parent.d + 1;
                    child.l = brother.r + cut_thickness;
                    child.r = parent.r;
                    child.b = parent.b;
                    child.t = parent.t;
                    child.item_type_id = -1;
                    parent.children.push_back(bin.nodes.size());
                    bin.nodes.push_back(child);
                    //std::cout << "new node " << child << std::endl;
                } else if (parent.t > brother.t + cut_thickness) {
                    SolutionNode child;
                    child.f = parent_id;
                    child.d = parent.d + 1;
                    child.l = parent.l;
                    child.r = parent.r;
                    child.b = brother.t + cut_thickness;
                    child.t = parent.t;
                    child.item_type_id = -1;
                    parent.children.push_back(bin.nodes.size());
                    bin.nodes.push_back(child);
                    //std::cout << "new node " << child << std::endl;
                }
            }
            parent_id = bin.nodes[parent_id].f;
            //std::cout << "parent_id " << parent_id << " depth " << bin.nodes[parent_id].d << std::endl;
        }
    }
    // Set residual.
    if (solution_.number_of_different_bins() > 0) {
        SolutionNode& node = solution_.bins_.back().nodes.back();
        if (node.item_type_id == -1 && node.d == 1)
            node.item_type_id = -3;
    }

    // Compute indicators.
    for (BinPos bin_pos = 0;
            bin_pos < solution_.number_of_different_bins();
            ++bin_pos) {
        solution_.update_indicators(bin_pos);
    }

    return std::move(solution_);
}
