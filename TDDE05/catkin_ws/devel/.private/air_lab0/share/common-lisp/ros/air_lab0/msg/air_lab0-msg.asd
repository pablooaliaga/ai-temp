
(cl:in-package :asdf)

(defsystem "air_lab0-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "ControllerEvaluationStat" :depends-on ("_package_ControllerEvaluationStat"))
    (:file "_package_ControllerEvaluationStat" :depends-on ("_package"))
  ))