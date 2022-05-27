#include "InfoDisplayer.h"

#include <MC/Actor.hpp>
#include <MC/Block.hpp>
#include <MC/Brightness.hpp>
#include <MC/CircuitSystem.hpp>
#include <MC/Dimension.hpp>
#include <MC/Material.hpp>
#include <MC/NavigationComponent.hpp>
#include <MC/Player.hpp>
#include <vector>

#include "DataConverter.h"
#include "GlobalServiceAPI.h"
#include "Msg.h"
#include "TrapdoorMod.h"

namespace tr {

    namespace {
        std::string getDbgString(Actor *actor) {
            std::vector<std::string> dbgs;
            actor->getDebugText(dbgs);
            std::string res;
            for (auto &s : dbgs) res += s;
            return res;
        }
    }  // namespace

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
            .sTextF(TextBuilder::GREEN, "%s    %llx\n",
                    a->getTypeName().c_str(), a->getUniqueID().get())

            .text(" - Pos/DeltaPos   AABB: ")
            .sTextF(TextBuilder::GREEN, "%s / %s|%s",
                    fromVec3(a->getPos()).toString().c_str(),
                    fromVec3(a->getPosDelta()).toString().c_str(),
                    fromAABB(a->getAABB()).ToString().c_str())
            .text("\n")
            .text(" - Surface: ")
            .sTextF(TextBuilder::GREEN, "%d\n", a->isSurfaceMob());

        builder.sText(TextBuilder::AQUA, "Dbg:\n")
            .textF("%s\n", getDbgString(a).c_str());
        builder.sText(tr::TextBuilder::AQUA, "NBT:\n")
            .textF("  %s", a->getNbt()->toSNBT().c_str());
        player->sendText(builder.get());
        return false;
    }

    bool displayerBlockInfo(Player *p, Block *b) {
        if (!p) return true;
        if (!b) {
            p->sendText("No block");
        }
        tr::TextBuilder builder;
        builder.sText(tr::TextBuilder::AQUA, "Base:\n")
            .text("Name / Type/ Id / Rtid: ")
            .sTextF(tr::TextBuilder::GREEN, " - %s / %s / %d / %d\n",
                    b->getName().c_str(), b->getTypeName().c_str(), b->getId(),
                    b->getRuntimeId())
            .text(" - canInstatick: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", b->canInstatick())
            .text(" - BlockEntity: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", b->hasBlockEntity());
        auto &m = b->getMaterial();
        builder.sText(tr::TextBuilder::AQUA, "Material:\n")
            .text(" - Motion: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", m.getBlocksMotion())

            .text(" - TopSolid: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", m.isTopSolid(true, true))
            .text(" - IsSolid: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", m.isSolid())
            .text(" - IsSolidBlocking: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", m.isSolidBlocking())
            .text(" - Translucency:")
            .sTextF(tr::TextBuilder::GREEN, "%.3f\n", m.getTranslucency());
        builder.sText(tr::TextBuilder::AQUA, "Debug:\n")
            .textF("  %s", b->toDebugString().c_str());
        builder.sText(tr::TextBuilder::AQUA, "NBT:\n")
            .textF("  %s", b->getNbt()->toSNBT().c_str());
        p->sendText(builder.get());
        return displayerRedstoneCompInfo(p, b);
    }
    bool displayerEnvInfo() { return true; }

    bool displayerRedstoneCompInfo(Player *p, Block *bi) { return false; }

}  // namespace tr
