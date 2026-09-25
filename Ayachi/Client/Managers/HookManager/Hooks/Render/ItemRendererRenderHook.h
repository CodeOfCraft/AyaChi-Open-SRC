#pragma once

#include "../FuncHook.h"

class ItemRenderer;
class BaseActorRenderContext;
struct ActorRenderData;

class ItemRendererRenderHook : public FuncHook {
private:
    using func_t = void(__fastcall*)(ItemRenderer*, BaseActorRenderContext&, ActorRenderData&);
    static func_t oFunc;
    static void renderCallback(
        ItemRenderer* renderer,
        BaseActorRenderContext& renderContext,
        ActorRenderData& renderData);

public:
    ItemRendererRenderHook();
};
