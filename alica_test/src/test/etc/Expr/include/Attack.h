#ifndef Attack_H_
#define Attack_H_
/*PROTECTED REGION ID(inc1402488848841) ENABLED START*/  //Add additional includes here
/*PROTECTED REGION END*/
namespace alica {
class Attack : public BasicBehaviour {
 public:
  Attack();
  virtual ~Attack();
  virtual void run(void* msg);
  /*PROTECTED REGION ID(pub1402488848841) ENABLED START*/  //Add additional public methods here
  /*PROTECTED REGION END*/
 protected:
  int callCounter;
  virtual void initialiseParameters();
  /*PROTECTED REGION ID(pro1402488848841) ENABLED START*/  //Add additional protected methods here
  /*PROTECTED REGION END*/
 private:
  /*PROTECTED REGION ID(prv1402488848841) ENABLED START*/  //Add additional private methods here
  /*PROTECTED REGION END*/};
} /* namespace alica */

#endif /* Attack_H_ */