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
#include "GlobalServiceAPI.h"
#include "Msg.h"
#include "TrAPI.h"
#include "TrapdoorMod.h"
#include "Utils.h"
namespace tr {

    namespace {}  // namespace

    bool displayEntityInfo(Player *player, Actor *a) {
        if (!player) return true;
        if (!a) {
            player->sendText("No actor");
            return true;
        }
        TextBuilder builder;
        builder
            .sText(TextBuilder::AQUA, "Base: \n")

            .text(" - type / UID: ")
            .sTextF(TextBuilder::GREEN, "%s    %llx\n", a->getTypeName().c_str(),
                    a->getUniqueID().get())

            .text(" - Pos/DeltaPos   AABB: ")
            .sTextF(TextBuilder::GREEN, "%s / %s|%s", fromVec3(a->getPos()).toString().c_str(),
                    fromVec3(a->getPosDelta()).toString().c_str(),
                    fromAABB(a->getAABB()).ToString().c_str())
            .text("\n")
            .text(" - Surface: ")
            .sTextF(TextBuilder::GREEN, "%d\n", a->isSurfaceMob());
        builder.sText(tr::TextBuilder::AQUA, "NBT:\n")
            .textF("  %s", a->getNbt()->toPrettySNBT(true).c_str());
        player->sendText(builder.get());
        return false;
    }

    ActionResult displayBlockInfo(Player *p, const BlockPos &position, bool nbt,
                                  const std::string &path) {
        if (!p) return {"Invalid Command Origin", false};
        auto pos = position;
        if (pos == tr::INVALID_POS) {
            pos = tr::getLookAtPos(p);
        }
        if (pos == tr::INVALID_POS) {
            return {"Get blockName failure", false};
        }
        auto &b = p->getRegion().getBlock(pos);
        tr::TextBuilder builder;
        if (nbt) {
            if (b.hasBlockEntity()) {
                auto be = p->getRegion().getBlockEntity(pos);
                if (be) {
                    auto sNbt = be->getNbt()->toSNBT();
                    if (sNbt.size() > 2256) {
                        return {sNbt, true};
                    } else {
                        return {be->getNbt()->toPrettySNBT(true), true};
                    }
                }
            } else {
                return {"No NBT data", false};
            }
        }

        builder.sText(tr::TextBuilder::AQUA, "Base:\n")
            .text(" - Name / Type: ")
            .sTextF(tr::TextBuilder::GREEN, "%s / %s\n", b.getName().c_str(),
                    b.getTypeName().c_str())
            .text(" - ID / RTID: ")
            .sTextF(tr::TextBuilder::GREEN, "%d / %d\n", b.getId(), b.getRuntimeId())
            .text(" - Variant: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", b.getVariant())
            .text(" - CanInstatick: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", b.canInstatick())
            .text(" - BlockEntity: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", b.hasBlockEntity())
            .text(" - IsSolid: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", b.isSolid());
        auto &m = b.getMaterial();
        builder.sText(tr::TextBuilder::AQUA, "Material:\n")
            .text(" - Motion: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", m.getBlocksMotion())
            .text(" - TopSolid: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", m.isTopSolid(false, false))
            .text(" - IsSolid: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", m.isSolid())
            .text(" - IsSolidBlocking: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", m.isSolidBlocking())
            .text(" - Translucency: ")
            .sTextF(tr::TextBuilder::GREEN, "%.3f\n", m.getTranslucency());
        return {builder.get(), true};
    }

    bool displayEnvInfo() { return true; }

    bool displayRedstoneCompInfo(Player *p, const BlockPos &pos) {
        auto &cs = p->getDimension().getCircuitSystem();
        auto &graph = getCircuitSceneGraph(&cs);
        auto comp = graph.getBaseComponent(pos);
        if (!comp) {
            p->sendText("Not an redstone component");
            return false;
        }
        p->sendText("Strength =>  " + std::to_string(comp->getStrength()));
        // blockName.getMobToSpawn("");
        return false;
    }
}  // namespace tr
