#pragma once

#include "ProfilerTask.h"
#include "imgui.h"
#include <array>
#include <map>
#include <vector>
#include <sstream>
#include <chrono>

#include "../../Utilities/Math/math.h"

namespace ImGuiProfileUtils
{
  inline dm::float2 Vec2(ImVec2 vec)
  {
    return dm::float2(vec.x, vec.y);
  }
  class ProfilerGraph
  {
  public:
    int frameWidth;
    int frameSpacing;
    bool useColoredLegendText;
    float maxFrameTime = 1.0f / 30.0f;

    ProfilerGraph(size_t framesCount);
    void LoadFrameData(const legit::ProfilerTask* tasks, size_t count);
    void RenderTimings(int graphWidth, int legendWidth, int height, int frameIndexOffset);

  private:
    void RebuildTaskStats(size_t endFrame, size_t framesCount);
    void RenderGraph(ImDrawList* drawList, dm::float2 graphPos, dm::float2 graphSize, size_t frameIndexOffset);
    void RenderLegend(ImDrawList* drawList, dm::float2 legendPos, dm::float2 legendSize, size_t frameIndexOffset);

    static void Rect(ImDrawList* drawList, dm::float2 minPoint, dm::float2 maxPoint, uint32_t col, bool filled = true);
    static void Text(ImDrawList* drawList, dm::float2 point, uint32_t col, const char* text);
    static void Triangle(ImDrawList* drawList, std::array<dm::float2, 3> points, uint32_t col, bool filled = true);
    static void RenderTaskMarker(ImDrawList* drawList, dm::float2 leftMinPoint, dm::float2 leftMaxPoint, dm::float2 rightMinPoint, dm::float2 rightMaxPoint, uint32_t col);
    
    struct FrameData
    {
      std::vector<legit::ProfilerTask> tasks;
      std::vector<size_t> taskStatsIndex;
    };

    struct TaskStats
    {
      double maxTime;
      size_t priorityOrder;
      size_t onScreenIndex;
    };

    std::vector<TaskStats> taskStats;
    std::map<std::string, size_t> taskNameToStatsIndex;
    std::vector<FrameData> frames;
    size_t currFrameIndex = 0;
  };

  class ProfilersWindow
  {
  public:
    ProfilersWindow();
    void Render();
    
    bool stopProfiling;
    int frameOffset;
    ProfilerGraph cpuGraph;
    ProfilerGraph gpuGraph;
    int frameWidth;
    int frameSpacing;
    bool useColoredLegendText;
    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
    TimePoint prevFpsFrameTime;
    size_t fpsFramesCount;
    float avgFrameTime;
  };
}

