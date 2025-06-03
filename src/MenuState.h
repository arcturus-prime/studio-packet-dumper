#pragma once

#include "imgui.h"

#include <vector>

namespace StudioDumper
{
class MenuState
{
    std::vector<std::vector<unsigned char>> packets;

  private:
    void drawHex(std::vector<unsigned char>& data)
    {
        size_t width = 16;

        for (size_t i = 1; i <= data.size(); i++)
        {
            ImGui::Text("%02x", data[i - 1]);

            if (i % width != 0 && i != data.size()) ImGui::SameLine();
        }
    }

  public:
    void draw()
    {
        ImGui::Begin("Packet Listing");

        for (size_t i = 0; i < this->packets.size(); i++)
        {
            ImGui::PushID(i);

            if (ImGui::CollapsingHeader("Packet"))
            {
                drawHex(this->packets[i]);
            }

            ImGui::PopID();
        }

        ImGui::End();
    }

    void add_packet(unsigned char* data, size_t size)
    {
        this->packets.push_back(std::vector<unsigned char>(data, data + size));
    }
};
} // namespace StudioDumper
