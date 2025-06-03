#pragma once

#include "imgui.h"

#include <vector>


namespace StudioDumper
{
class MenuState
{
    std::vector<std::vector<unsigned char>> packets;

  public:
    void render()
    {
        ImGui::Begin("Dumper");
        if (ImGui::CollapsingHeader("Some Packet"))
        {
            ImGui::Text("Test");
        }
        ImGui::End();
    }

    void add_packet(unsigned char* data, size_t size) {
        this->packets.push_back(std::vector<unsigned char>(data, data + size));
    }
};
} // namespace StudioDumper
