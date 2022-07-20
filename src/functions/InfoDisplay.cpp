#include "InfoDisplay.h"

#include <MC/Actor.hpp>
#include <MC/BaseCircuitComponent.hpp>
#include <MC/Biome.hpp>
#include <MC/Block.hpp>
#include <MC/BlockActor.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Brightness.hpp>
#include <MC/CircuitSceneGraph.hpp>
#include <MC/CircuitSystem.hpp>
#include <MC/Dimension.hpp>
#include <MC/Material.hpp>
#include <MC/NavigationComponent.hpp>
#include <MC/Player.hpp>
#include <vector>

#include "CommandHelper.h"
#include "DataConverter.h"
#include "Global.h"
#include "MC/PrettySnbtFormat.hpp"
#include "Msg.h"
#include "Particle.h"
#include "TrAPI.h"
#include "TrapdoorMod.h"
#include "Utils.h"
namespace trapdoor {
    namespace {

        struct ComponentItem {
            BaseCircuitComponent *mComponent = nullptr;  // 0 * 4 - 1 * 4
            int mDampening = 0;                          // 2 * 4
            BlockPos mPos;                               // 3 * 4 - 5 * 4
            unsigned char facing{};                      // 6 * 4
            bool mDirectlyPowered = false;               // 6* 4
            int mData = 0;                               // 7*4
        };

        static_assert(sizeof(ComponentItem) == 32);

        struct ActorNBTFormat : PrettySnbtFormat {
            ActorNBTFormat() {
                this->switchToPlayerFormat();
                this->mMaxLevel = 1;
            }
        };

        std::string printableNBT(const std::unique_ptr<CompoundTag> &nbt) {
            return nbt->toPrettySNBT(true);
        }
        ActionResult getNBTString(const std::unique_ptr<CompoundTag> &nbt,
                                  const std::string &path) {
            if (path.empty()) {
                return {printableNBT(nbt), true};
            } else {
                bool success = false;
                auto str = getNBTInfoFromPath(nbt, path, success);
                return {str, success};
            }
        }
    }  // namespace
    ActionResult displayEntityInfo(Player *player, Actor *a, bool nbt, const std::string &path) {
        if (!player) return ErrorPlayerNeed();
        if (!a) {
            return {"No actor", false};
        }
        TextBuilder builder;
        if (nbt) {
            return getNBTString(a->getNbt(), path);
        }

        builder.sText(TextBuilder::AQUA, "Base: \n")
            .text(" - type / UID: ")
            .sTextF(TextBuilder::GREEN, "%s    %llx\n", a->getTypeName().c_str(),
                    a->getUniqueID().get())
            .text(" - Pos/DeltaPos: ")
            .sTextF(TextBuilder::GREEN, "%s / %s|%s", fromVec3(a->getPos()).toString().c_str(),
                    fromVec3(a->getPosDelta()).toString().c_str())
            .text(" - AABB: ")
            .sTextF(TextBuilder::GREEN, "%s", fromAABB(a->getAABB()).ToString().c_str())
            .text("\n")
            .text(" - Surface: ")
            .sTextF(TextBuilder::GREEN, "%d\n", a->isSurfaceMob());
        return {builder.get(), true};
    }

    ActionResult displayBlockInfo(Player *p, const BlockPos &position, bool nbt,
                                  const std::string &path) {
        if (!p) return ErrorPlayerNeed();
        auto pos = position;
        if (pos == BlockPos::MAX) {
            pos = trapdoor::getLookAtPos(p);
        }

        if (pos == BlockPos::MAX) {
            return {"Get blockName failure", false};
        }
        auto &b = p->getRegion().getBlock(pos);
        trapdoor::TextBuilder builder;
        if (nbt) {
            if (b.hasBlockEntity()) {
                auto be = p->getRegion().getBlockEntity(pos);
                if (be) {
                    return getNBTString(be->getNbt(), path);
                }
            } else {
                return {"No NBT data", false};
            }
        }

        builder.sText(trapdoor::TextBuilder::AQUA, "Base:\n")
            .text(" - Name / Type: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%s / %s\n", b.getName().c_str(),
                    b.getTypeName().c_str())
            .text(" - ID / RTID: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d / %d\n", b.getId(), b.getRuntimeId())
            .text(" - Variant: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d\n", b.getVariant())
            .text(" - CanInstatick: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d\n", b.canInstatick())
            .text(" - BlockEntity: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d\n", b.hasBlockEntity())
            .text(" - IsSolid: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d\n", b.isSolid());
        auto &m = b.getMaterial();
        builder.sText(trapdoor::TextBuilder::AQUA, "Material:\n")
            .text(" - Motion: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d\n", m.getBlocksMotion())
            .text(" - TopSolid: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d\n", m.isTopSolid(false, false))
            .text(" - IsSolid: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d\n", m.isSolid())
            .text(" - IsSolidBlocking: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d\n", m.isSolidBlocking())
            .text(" - Translucency: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%.3f\n", m.getTranslucency());
        return {builder.get(), true};
    }

    bool displayEnvInfo() { return true; }

    ActionResult displayRedstoneCompInfo(Dimension *d, const BlockPos &pos) {
        if (!d) return ErrorDimension();
        auto &cs = d->getCircuitSystem();
        auto &graph = getCircuitSceneGraph(&cs);
        auto comp = graph.getBaseComponent(pos);
        if (!comp) {
            return {"Not an redstone component", false};
        }

        TextBuilder builder;
        builder.text("Signal: ").num(comp->getStrength()).text("\n");
        auto &list = dAccess<std::vector<ComponentItem>, 8>(comp);
        for (auto &source : list) {
            auto p = source.mPos;
            builder.textF("P: [%s] D: %d S: %d\n", p.toString().c_str(), source.mDampening,
                          source.mComponent->getStrength());
            auto color = source.mDirectlyPowered ? PCOLOR::YELLOW : PCOLOR::GREEN;
            trapdoor::shortHighlightBlock({p.x, p.y, p.z}, color, d->getDimensionId());
        }

        return {builder.get(), true};
    }
}  // namespace trapdoor
