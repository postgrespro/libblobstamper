/******************************************************************************
 *
 * Copyright 2021 Nikolay Shaplov (Postgres Professional)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#ifndef BLOB_H
#define BLOB_H

#include"dict.h"


size_t
DictBase::size()
{
    return data.size();
}

std::string
DictBase::get(size_t n)
{
    return data[n];
}


DictLCAlphaSmall::DictLCAlphaSmall()
{
/* List below was generated with following perl script

open(my $fh, "/usr/share/dict/british-english-small");
my $i=0;
while (my $s=<$fh>)
{
  chomp $s;
  if ($s =~ /^[a-z]+$/)
  {
    print '"'.$s.'"'.",\n" if $i % 157 == 0;
    $i++;
  }
}
*/
    data = {
"a",
"abundances",
"achieves",
"adjunct",
"affair",
"aired",
"ally",
"amphitheatre",
"annoyance",
"appalled",
"arbitrators",
"artichokes",
"assisting",
"auctioneers",
"awaked",
"balanced",
"barbiturates",
"bawdier",
"beggaring",
"berserk",
"biographies",
"blazer",
"blown",
"bond",
"boundless",
"breaded",
"broadsided",
"buggy",
"burping",
"caches",
"caned",
"cards",
"castoffs",
"cellar",
"chant",
"cheetahs",
"choral",
"citizen",
"cleft",
"clues",
"coined",
"commanding",
"competences",
"concentrate",
"configures",
"conserve",
"contemptuous",
"convertors",
"corpulent",
"counterparts",
"cranking",
"crippled",
"crusades",
"cursing",
"dams",
"debate",
"dedication",
"deign",
"denomination",
"descents",
"detonate",
"differences",
"dirt",
"discouragement",
"dismally",
"dissented",
"divested",
"doorstep",
"dread",
"drunks",
"dying",
"edifices",
"elegant",
"emergency",
"encyclopedia",
"enshrine",
"epitaph",
"eternities",
"excavates",
"exists",
"extends",
"fact",
"fashionably",
"fellow",
"fifteen",
"firms",
"flavourings",
"floured",
"followers",
"forestalled",
"fouler",
"freshens",
"fulfilling",
"galaxy",
"gaudier",
"germs",
"gleam",
"godfather",
"graffiti",
"greened",
"grouping",
"gunshots",
"halved",
"harmonic",
"headrest",
"hems",
"hims",
"holy",
"horrify",
"humans",
"hyphenates",
"illustration",
"impersonated",
"inadequate",
"increase",
"industrious",
"infuriate",
"inquiry",
"insulated",
"intern",
"intuitive",
"irreverence",
"jaws",
"journals",
"kegs",
"kneecap",
"lamentations",
"launder",
"lefts",
"liberated",
"lingoes",
"lobes",
"lords",
"lushes",
"maintaining",
"mannequins",
"mascot",
"meadows",
"men",
"miaows",
"minds",
"misjudge",
"modernise",
"mop",
"mourners",
"muscles",
"narrations",
"neglects",
"nightgowns",
"nostalgia",
"nymph",
"oceans",
"ooze",
"ore",
"outlasted",
"overgrowing",
"overwriting",
"pamper",
"pardon",
"pastiche",
"pearl",
"people",
"perplexity",
"phenomenal",
"pigged",
"pivots",
"pleasured",
"pokers",
"portables",
"powders",
"preface",
"pretzels",
"procedures",
"prom",
"protective",
"puddles",
"purr",
"queasier",
"racquets",
"ranks",
"readable",
"receptionist",
"recuperated",
"refrigerate",
"reiterate",
"reminisces",
"replenishes",
"resemblances",
"resulted",
"reverenced",
"ridiculing",
"robots",
"route",
"rustlers",
"salves",
"savour",
"schoolboy",
"scrounging",
"sedating",
"separates",
"shakiest",
"shined",
"shrewdness",
"significance",
"sitters",
"slakes",
"slogged",
"smoothest",
"snowfalls",
"solidifies",
"sow",
"speeches",
"spongiest",
"squalled",
"stammer",
"stealthy",
"stitches",
"strangers",
"stubbiest",
"subside",
"suitably",
"supplemented",
"swampiest",
"sympathetically",
"taken",
"tattled",
"temperate",
"testicle",
"thirteenths",
"tickles",
"tittered",
"tormentors",
"tragic",
"trappings",
"triggers",
"trusty",
"twigged",
"unburdening",
"undeserved",
"unite",
"unseemly",
"upstream",
"valved",
"verbiage",
"vindicating",
"vortexes",
"want",
"waxes",
"wets",
"whom",
"wipes",
"woolliest",
"wrinkles",
"zanier"
 };
}

#endif  /* DICT_H*/