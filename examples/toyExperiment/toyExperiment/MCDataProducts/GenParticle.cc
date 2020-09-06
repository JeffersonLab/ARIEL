//
// A minimal class to hold information about generated particles.
//

#include "toyExperiment/MCDataProducts/GenParticle.h"
#include <iostream>

tex::GenParticle::GenParticle()
  : _pdgId(PDGCode::invalid), _parent(), _children(), _position(), _momentum()
{}

tex::GenParticle::GenParticle(PDGCode::type pdgId,
                              art::Ptr<GenParticle> const& parent,
                              CLHEP::Hep3Vector const& position,
                              CLHEP::HepLorentzVector const& momentum)
  : _pdgId(pdgId)
  , _parent(parent)
  , _children()
  , _position(position)
  , _momentum(momentum)
{}

void
tex::GenParticle::addChild(art::Ptr<GenParticle> const& child)
{
  _children.push_back(child);
}

std::ostream&
tex::operator<<(std::ostream& ost, const tex::GenParticle& genp)
{
  ost << "[ "
      << "pdg: " << genp.pdgId() << " "
      << "Position: " << genp.position() << " "
      << "4-momentum " << genp.momentum() << " "
      << "parent: ";
  if (genp.parent().id() == art::ProductID()) {
    ost << "none ";
  } else {
    ost << genp.parent().id() << "." << genp.parent().key() << " ";
  }
  ost << " children: ";

  if (genp.children().empty()) {
    ost << "none";
  } else {
    ost << "( ";
    for (auto const& child : genp.children()) {
      ost << " " << child.id() << "." << child.key();
    }
    ost << " )";
  }
  ost << " ]";
  return ost;
}
