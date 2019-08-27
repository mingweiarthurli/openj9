class AbstractValueInterface
{
public:
  virtual void lub(const AbstractValueInterface&) = 0;
  virtual void glb(const AbstractValueInterface&) = 0;
  virtual void trace() = 0;
};

class AbstractStateInterface
{
public:
  virtual void lub(const AbstractStateInterface&) = 0;
  virtual void glb(const AbstractStateInterface&) = 0;
  virtual void trace() = 0;
};
