#include "LibraryAnimationsReader.h"
#include "../Types/Common/Source.h"
#include "XML/XMLUtils.h"
#include "Logger/Log.h"
#include "Utils.h"
#include <rapidxml.hpp>

namespace GameEngine
{
	namespace Collada
	{
		void LibraryAnimationsReader::read(XMLNode * node)
		{
			Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode)
			{
				if (data.name == "animation")
				{
					ProcessAnimation(data_.animationsClips_["DefaultAnimationClip"], snode);
				}
				if (data.name == "animationClip")
				{
					auto animationClipName = Utils::GetRapidAttributeData(snode, "name").value;
					ProcessAnimationClip(data_.animationsClips_[animationClipName], snode);
				}
			});
		}
		void LibraryAnimationsReader::ProcessAnimationClip(AnimationClip& animation, XMLNode * node)
		{
			Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* animClipnode)
			{
				if (data.name == "animation")
				{
					ProcessAnimation(animation, animClipnode);
				}
			});
		}
		void LibraryAnimationsReader::ProcessAnimation(AnimationClip& animationClip, XMLNode* node)
		{
			auto aid = Utils::GetRapidAttributeData(node, "id").value;

			if (aid.empty())
				return;

			auto& animation = animationClip.animations_[aid];
			animation.id_ = aid;

			Utils::ForEachSubNode(node, [&](const Utils::RapidNodeData& data, XMLNode* snode)
			{
				if (data.name == "source")
				{
					auto sourceId = Utils::GetRapidAttributeData(snode, "id").value;
					if (sourceId.empty())
						return;

					ProcessSource(snode, animation.sources_[sourceId]);
				}
				else if (data.name == "sampler")
				{
					animation.sampler_.id_ = Utils::GetRapidAttributeData(snode, "id").value;
					Utils::ForEachSubNode(snode, [&](const Utils::RapidNodeData& data, XMLNode* inputnode)
					{
						animation.sampler_.inputs_.emplace_back();
						ProcessInput(inputnode, animation.sampler_.inputs_.back());
					});
					
				}
				else if (data.name == "channel")
				{
					animation.channel_.source_ = Utils::GetRapidAttributeData(snode, "source").value;
					animation.channel_.target_ = Utils::GetRapidAttributeData(snode, "target").value;
				}
			});
		}
	} // Collada
} // GameEngine
